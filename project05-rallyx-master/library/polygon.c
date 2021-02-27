#include "polygon.h"

double polygon_area(list_t *polygon) {
    double result = 0;
    // Add the cross products of all of the consecutive pairs (except last one)
    for (size_t i = 0; i < list_size(polygon) - 1; i++) {
        vector_t *current = list_get(polygon, i);
        vector_t *next = list_get(polygon, i + 1);
        result += vec_cross(*current, *next);
    }
    // Add in last cross product for wrap around term
    size_t last_idx = list_size(polygon) - 1;
    vector_t *last = list_get(polygon, last_idx);
    vector_t *first = list_get(polygon, 0);
    result += vec_cross(*last, *first);

    return fabs(result) / 2;

}

vector_t polygon_centroid(list_t *polygon) {
    double area = polygon_area(polygon);
    double x = 0;
    double y = 0;
    for (size_t i = 0; i < list_size(polygon) - 1; i++) {
        vector_t *current = list_get(polygon, i);
        vector_t *next = list_get(polygon, i + 1);
        x += (current->x + next->x) *
            ((current->x * next->y) - (next->x * current->y));
        y += (current->y + next->y) *
            ((current->x * next->y) - (next->x * current->y));
    }

    size_t last_idx = list_size(polygon) - 1;
    vector_t *last = list_get(polygon, last_idx);
    vector_t *first = list_get(polygon, 0);
    x += (last->x + first->x) * ((last->x * first->y) - (first->x * last-> y));
    y += (last->y + first->y) * ((last->x * first->y) - (first->x * last-> y));


    x /= (6 * area);
    y /= (6 * area);

    vector_t result = {
        .x = x,
        .y = y
    };

    return result;
}

void polygon_translate(list_t *polygon, vector_t translation) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *current = list_get(polygon, i);
        *(current) = vec_add(*(current), translation);
    }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *current = list_get(polygon, i);
        vector_t shift = vec_subtract(*current, point);
        vector_t rotation = vec_rotate(shift, angle);
        *current = vec_add(rotation, point);
    }
}
