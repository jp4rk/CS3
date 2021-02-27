#include "body.h"
#include "color.h"

struct body {
    list_t *shape;
    vector_t velocity;
    vector_t centroid;
    rgb_color_t color;
    double mass;
    double angle;
};

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    body_t *result = malloc(1 * sizeof(body_t));
    assert(result != NULL);
    result->shape = shape;
    result->velocity = VEC_ZERO;
    result->centroid = polygon_centroid(shape);
    result->color = color;
    result->mass = mass;
    result->angle = 0;
    return result;
}

void body_free(body_t *body) {
    list_free(body->shape);
    free(body);
}

list_t* body_get_shape(body_t *body) {
    list_t *new_list = list_init(list_size(body->shape),
        (free_func_t) free);
    for (size_t i = 0; i < list_size(body->shape); i++) {
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = *((vector_t*) list_get(body->shape, i));
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

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

double body_get_angle(body_t *body) {
    return body->angle;
}

void body_set_centroid(body_t *body, vector_t x) {
    polygon_translate(body->shape,
        vec_subtract(x, body->centroid));
    body->centroid = x;
}

void body_set_velocity(body_t *shape, vector_t v) {
    shape->velocity = v;
}

void body_set_rotation(body_t *body, double angle) {
    polygon_rotate(body->shape, angle - body->angle,
        polygon_centroid(body->shape));
    body->angle = angle;
}

void body_tick(body_t *body, double dt) {
    vector_t translation = {
        .x = body->velocity.x * dt,
        .y = body->velocity.y * dt
    };
    body_set_centroid(body, vec_add(body->centroid, translation));
}
