#include "collision.h"

vector_t project_shape(list_t *shape, vector_t line) {
  vector_t *first = list_get(shape, 0);
  double max_projection = vec_projection(*first, line);
  double min_projection = vec_projection(*first, line);
  for (size_t i = 1; i < list_size(shape); i++) {
      vector_t *current = list_get(shape, i);
      double current_projection = vec_projection(*current, line);
      if (current_projection < min_projection) {
          min_projection = current_projection;
      }
      if (current_projection > max_projection) {
          max_projection = current_projection;
      }
  }
  vector_t result = {
      .x = min_projection,
      .y = max_projection
  };
  return result;
}

double find_overlap(list_t *shape1, list_t *shape2, vector_t line) {
    // Find min and max values for shape1
    vector_t projection1 = project_shape(shape1, line);

    // Find min and max values for shape2
    vector_t projection2 = project_shape(shape2, line);

    // Check if there is a gap between these two min, max ranges
    if (projection1.x <= projection2.x && projection1.y >= projection2.x) {
        return (fmin(projection1.y, projection2.y) - projection2.x);
    }
    if (projection1.x >= projection2.x && projection2.y >= projection1.x) {
        return (fmin(projection2.y, projection1.y) - projection1.x);
    }
    return 0;
}

vector_t check_edges(list_t *shape1, list_t *shape2) {
    vector_t *current = list_get(shape1, 0);
    vector_t *next = list_get(shape1, 1);
    vector_t diff = vec_subtract(*next, *current);
    vector_t line = vec_perpendicular(diff);
    double overlap = find_overlap(shape1, shape2, line);
    if (overlap == 0) {
        return VEC_ZERO;
    }
    vector_t collision_axis = vec_unit(line);
    double min_overlap = overlap;

    // Iterate through all edges and find minimum overlap on perpendicular lines
    for (size_t i = 1; i < list_size(shape1) - 1; i++) {
        vector_t *current = list_get(shape1, i);
        vector_t *next = list_get(shape1, i + 1);
        vector_t diff = vec_subtract(*next, *current);
        vector_t line = vec_perpendicular(diff);
        double overlap = find_overlap(shape1, shape2, line);
        if (overlap == 0) {
            return VEC_ZERO;
        }
        if (overlap < min_overlap) {
            collision_axis = vec_unit(line);
            min_overlap = overlap;
        }
    }

    for (size_t j = 0; j < list_size(shape2) - 1; j++) {
        vector_t *current = list_get(shape2, j);
        vector_t *next = list_get(shape2, j + 1);
        vector_t diff = vec_subtract(*next, *current);
        vector_t line = vec_perpendicular(diff);
        double overlap = find_overlap(shape1, shape2, line);
        if (overlap == 0) {
            return VEC_ZERO;
        }
        if (overlap < min_overlap) {
            collision_axis = vec_unit(line);
            min_overlap = overlap;
        }
    }

    return collision_axis;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
    collision_info_t info = {
        .collided = false,
        .axis = VEC_ZERO
    };

    vector_t collision_axis = check_edges(shape1, shape2);

    if (vec_equal(collision_axis, VEC_ZERO)) {
        return info;
    }

    info.collided = true;
    info.axis = collision_axis;

    return info;

}
