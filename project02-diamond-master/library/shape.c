#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "shape.h"
#include "vec_list.h"
#include "vector.h"
#include "polygon.h"

// Returns a veclist containing the points vector of shape
vec_list_t *shape_get_points(shape_t *shape){
  return *shape->points;
}

// Returns a double returning the red value of shape
double shape_get_r(shape_t *shape){
  return shape->r;
}

// Returns a double returning the green value of shape
double shape_get_g(shape_t *shape){
  return shape->g;
}

// Returns a double returning the blue value of shape
double shape_get_b(shape_t *shape){
  return shape->b;
}

// Returns a list corresponding to the points vector of shape
vector_t *shape_get_velocity(shape_t *shape){
  return shape->velocity;
}

// Returns a double corresponding to the outer radius of shape
double shape_get_outer_radius(shape_t *shape){
  return shape->outer_radius;
}

// Returns a double corresponding to the outer radius of shape
double shape_get_inner_radius(shape_t *shape){
  return shape->inner_radius;
}

// Frees the memory for the shape
void shape_free(shape_t *shape){
  vec_list_free(*(shape->points));
  free(shape->points);
  free(shape->velocity);
  // free(shape->velocity);
  free(shape);
}
