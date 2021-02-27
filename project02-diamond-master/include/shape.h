#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "vec_list.h"

typedef struct shape{
  vec_list_t **points;
  double r;
  double g;
  double b;
  vector_t *velocity;
  double outer_radius;
  double inner_radius;
} shape_t;

vec_list_t* shape_get_points(shape_t *shape);

void shape_free(shape_t *shape);

double shape_get_r(shape_t *shape);

double shape_get_g(shape_t *shape);

double shape_get_b(shape_t *shape);

vector_t* shape_get_velocity(shape_t *shape);

double shape_get_radius(shape_t *shape);

#endif
