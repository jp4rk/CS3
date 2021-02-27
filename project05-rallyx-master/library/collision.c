#include "collision.h"

bool check_overlap(list_t *shape1, list_t *shape2, vector_t line) {
    // Find min and max values for shape1
    vector_t *first1 = list_get(shape1, 0);
    double max_projection1 = vec_projection(*first1, line);
    double min_projection1 = vec_projection(*first1, line);
    for (size_t i = 1; i < list_size(shape1); i++) {
        vector_t *current = list_get(shape1, i);
        double current_projection = vec_projection(*current, line);
        if (current_projection < min_projection1) {
            min_projection1 = current_projection;
        }
        if (current_projection > max_projection1) {
            max_projection1 = current_projection;
        }
    }

    // Find min and max values for shape2
    vector_t *first2 = list_get(shape2, 0);
    double max_projection2 = vec_projection(*first2, line);
    double min_projection2 = vec_projection(*first2, line);
    for (size_t j = 1; j < list_size(shape2); j++) {
        vector_t *current = list_get(shape2, j);
        double current_projection = vec_projection(*current, line);
        if (current_projection < min_projection2) {
            min_projection2 = current_projection;
        }
        if (current_projection > max_projection2) {
            max_projection2 = current_projection;
        }
    }

    // Check if there is a gap between these two min, max ranges
    if (min_projection1 <= min_projection2 && max_projection1 >= min_projection2) {
        return true;
    }
    if (min_projection1 >= min_projection2 && max_projection2 >= min_projection1) {
        return true;
    }
    return false;
}

bool find_collision(list_t *shape1, list_t *shape2) {
    // Iterate through all edges and check for overlap on perpendicular lines
    for (size_t i = 0; i < list_size(shape1) - 1; i++) {
        vector_t *current = list_get(shape1, i);
        vector_t *next = list_get(shape1, i + 1);
        vector_t diff = vec_subtract(*next, *current);
        vector_t line = vec_perpendicular(diff);
        if (check_overlap(shape1, shape2, line) == false) {
            return false;
        }
    }
    for (size_t j = 0; j < list_size(shape2) - 1; j++) {
        vector_t *current = list_get(shape2, j);
        vector_t *next = list_get(shape2, j + 1);
        vector_t diff = vec_subtract(*next, *current);
        vector_t line = vec_perpendicular(diff);
        if (check_overlap(shape1, shape2, line) == false) {
            return false;
        }
    }
    return true;
}
