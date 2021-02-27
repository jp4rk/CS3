#include "forces.h"

struct force_args {
    double constant;
    body_t *body1;
    body_t *body2;
};

struct collision_args {
    collision_handler_t handler;
    body_t *body1;
    body_t *body2;
    void *aux;
    free_func_t freer;
    bool collided_last_tick;
};

typedef struct constant_args {
    double constant;
} constant_args_t;

force_args_t *force_args_init(double constant, body_t *body1, body_t *body2) {
    force_args_t *result = malloc(1 * sizeof(force_args_t));
    result->constant = constant;
    result->body1 = body1;
    result->body2 = body2;
    return result;
}

collision_args_t *collision_args_init(collision_handler_t handler, body_t *body1,
    body_t *body2, void *aux, free_func_t freer) {
    collision_args_t *result = malloc(sizeof(collision_args_t));
    result->handler = handler;
    result->body1 = body1;
    result->body2 = body2;
    result->aux = aux;
    result->freer = freer;
    return result;
}

void collision_args_free(collision_args_t *args) {
    if (args->freer != NULL) {
        args->freer(args->aux);
    }
    free(args);
}

void newtonian_gravity_force(force_args_t *arg) {
    vector_t result = VEC_ZERO;
    vector_t difference =
        vec_subtract(body_get_centroid(arg->body2), body_get_centroid(arg->body1));
    double distance = sqrt(difference.x * difference.x + difference.y * difference.y);
    if (distance > 1) {
        double coef = -1 * arg->constant * body_get_mass(arg->body1) *
                      body_get_mass(arg->body2) / (distance * distance);
        vector_t unit_vec = vec_multiply(1 / distance, difference);
        result = vec_multiply(coef, unit_vec);
    }
    body_add_force(arg->body2, result);
    body_add_force(arg->body1, vec_negate(result));
}

void create_newtonian_gravity(scene_t *scene, double gravity, body_t *body1,
                              body_t *body2) {
    force_args_t *args = force_args_init(gravity, body1, body2);
    list_t * bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, (force_creator_t)newtonian_gravity_force, args,
                            bodies, free);
}

void spring_force(force_args_t *arg) {
    vector_t difference =
        vec_subtract(body_get_centroid(arg->body2), body_get_centroid(arg->body1));
    vector_t result = vec_multiply(arg->constant, difference);
    body_add_force(arg->body1, result);
    body_add_force(arg->body2, vec_negate(result));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
    force_args_t *args = force_args_init(k, body1, body2);
    list_t * bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, (force_creator_t)spring_force, args,
                            bodies, free);
}

void drag_force(force_args_t *arg) {
    vector_t velo = body_get_velocity(arg->body1);
    vector_t result = vec_multiply(-1 * arg->constant, velo);
    body_add_force(arg->body1, result);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
    force_args_t *args = force_args_init(gamma, body, body);
    list_t * bodies = list_init(1, NULL);
    list_add(bodies, body);
    scene_add_bodies_force_creator(scene, (force_creator_t)drag_force, args, bodies,
        free);
}

void apply_collision(collision_args_t *args) {
    list_t *shape1 = body_get_shape(args->body1);
    list_t *shape2 = body_get_shape(args->body2);
    collision_info_t collision = find_collision(shape1, shape2);
    if (collision.collided && !args->collided_last_tick) {
        args->handler(args->body1, args->body2, collision.axis, args->aux);
    }
    args->collided_last_tick = collision.collided;
    list_free(shape1);
    list_free(shape2);
}

// This function is incomplete! Confused about how to implement.
void create_collision(scene_t *scene, body_t *body1, body_t *body2,
    collision_handler_t handler, void *aux, free_func_t freer) {
    collision_args_t *args = collision_args_init(handler, body1, body2, aux, freer);
    list_t * bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    scene_add_bodies_force_creator(scene, (force_creator_t)apply_collision, args,
        bodies, (free_func_t)collision_args_free);
}

void destructive_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_remove(body1);
    body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2) {
    create_collision(scene, body1, body2, (collision_handler_t)destructive_collision,
        NULL, NULL);
}

void physics_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    constant_args_t *c = (constant_args_t *) aux;
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);
    double u1 = vec_dot(body_get_velocity(body1), axis);
    double u2 = vec_dot(body_get_velocity(body2), axis);
    double reduced_mass = (m1 * m2) / (m1 + m2);
    if (m1 == INFINITY) {
        reduced_mass = m2;
    }
    else if (m2 == INFINITY) {
        reduced_mass = m1;
    }
    double magnitude = reduced_mass * (1 + c->constant) * (u2 - u1);
    vector_t impulse1 = vec_multiply(magnitude, axis);
    vector_t impulse2 = vec_multiply(-1, impulse1);
    body_add_impulse(body1, impulse1);
    body_add_impulse(body2, impulse2);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1,
    body_t *body2) {
    constant_args_t * arg = malloc(sizeof(constant_args_t));
    arg->constant = elasticity;
    create_collision(scene, body1, body2, (collision_handler_t)physics_collision,
        arg, free);
}

void physics_destructive_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    physics_collision(body1, body2, axis, aux);
    body_remove(body2);
}

void create_physics_destructive_collision(scene_t *scene, double elasticity, body_t *body1,
    body_t *body2) {
      constant_args_t * arg = malloc(sizeof(constant_args_t));
      arg->constant = elasticity;
      create_collision(scene, body1, body2, (collision_handler_t)physics_destructive_collision,
          arg, free);
}
