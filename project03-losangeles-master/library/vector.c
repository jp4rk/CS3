#include "vector.h"

const vector_t VEC_ZERO = {
    .x = 0,
    .y = 0
};

vector_t vec_add(vector_t v1, vector_t v2) {
    vector_t vec = {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y
    };
    return vec;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
    vector_t vec = {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y
    };
    return vec;
}

vector_t vec_negate(vector_t v) {
    vector_t vec = {
        .x = -v.x,
        .y = -v.y
    };
    return vec;
}

vector_t vec_multiply(double scalar, vector_t v) {
    vector_t vec = {
        .x = scalar * v.x,
        .y = scalar * v.y
    };
    return vec;
}

double vec_dot(vector_t v1, vector_t v2) {
    double res = v1.x * v2.x + v1.y * v2.y;
    return res;
}

double vec_cross(vector_t v1, vector_t v2) {
    double res = v1.x * v2.y - v1.y * v2.x;
    return res;
}

vector_t vec_rotate(vector_t v, double angle) {
    vector_t vec = {
        v.x * cos(angle) - v.y * sin(angle),
        v.x * sin(angle) + v.y * cos(angle)
    };
    return vec;
}
