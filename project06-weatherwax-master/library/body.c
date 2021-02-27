#include "body.h"

struct body {
    list_t *shape;
    vector_t velocity;
    vector_t acceleration;
    vector_t centroid;
    rgb_color_t color;
    double mass;
    double angle;
    vector_t impulse;
    vector_t force;
    void *info;
    free_func_t info_freer;
    bool removed;
};

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
    void *info, free_func_t info_freer) {
      body_t *result = malloc(1 * sizeof(body_t));
      assert(result != NULL);
      result->shape = shape;
      result->velocity = VEC_ZERO;
      result->acceleration = VEC_ZERO;
      result->centroid = polygon_centroid(shape);
      result->color = color;
      result->mass = mass;
      result->angle = 0;
      result->impulse = VEC_ZERO;
      result->force = VEC_ZERO;
      result->info = info;
      result->info_freer = info_freer;
      result->removed = false;
      return result;
}

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

void body_free(body_t *body) {
    list_free(body->shape);
    if (body->info_freer != NULL) {
        body->info_freer(body->info);
    }
    free(body);
}

list_t *body_get_shape(body_t *body) {
    list_t *new_list = list_init(list_size(body->shape), (free_func_t)free);
    for (size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = *((vector_t *)list_get(body->shape, i));
        list_add(new_list, new_vec);
    }
    return new_list;
}

vector_t body_get_centroid(body_t *body) {
    return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
    return body->velocity;
}

vector_t body_get_acceleration(body_t *body) {
    return body->acceleration;
}

double body_get_mass(body_t *body) {
    return body->mass;
}

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

double body_get_angle(body_t *body) {
    return body->angle;
}

void *body_get_info(body_t *body) {
    return body->info;
}

free_func_t body_get_info_freer(body_t *body) {
    return body->info_freer;
}

void body_set_centroid(body_t *body, vector_t x) {
    polygon_translate(body->shape, vec_subtract(x, body->centroid));
    body->centroid = x;
}

void body_set_velocity(body_t *body, vector_t v) {
    body->velocity = v;
}

void body_set_rotation(body_t *body, double angle) {
    polygon_rotate(body->shape, angle - body->angle, polygon_centroid(body->shape));
    body->angle = angle;
}

void body_add_force(body_t *body, vector_t force) {
    body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
    body->impulse = vec_add(body->impulse, impulse);
}

void body_remove(body_t *body) {
    body->removed = true;
}

bool body_is_removed(body_t *body) {
    return body->removed;
}

void body_tick(body_t *body, double dt) {
    body->acceleration = vec_multiply(1 / body->mass, body->force);
    vector_t before_tick = body->velocity;
    vector_t velo_change = vec_multiply(dt, body->acceleration);
    vector_t impulse_change = vec_multiply(1 / body->mass, body->impulse);
    body->velocity = vec_add(body->velocity, vec_add(velo_change, impulse_change));
    vector_t avg = vec_average(before_tick, body->velocity);
    vector_t translation = vec_multiply(dt, avg);

    body->impulse = VEC_ZERO;
    body->force = VEC_ZERO;
    body_set_centroid(body, vec_add(body->centroid, translation));
}
