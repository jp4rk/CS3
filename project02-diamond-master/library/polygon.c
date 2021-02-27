#include <stdio.h>
#include "polygon.h"

const int AREAFACTOR = 6;

// Returns the area of the polygon
double polygon_area(vec_list_t *polygon){
  double sum = 0.0;
  for(size_t i = 0; i < vec_list_size(polygon)-1; ++i){
    vector_t *val_one = vec_list_get(polygon, i);
    vector_t *val_two = vec_list_get(polygon, i+1);
    sum += val_one -> x * val_two -> y - val_two -> x * val_one -> y;
  }
  vector_t *last = vec_list_get(polygon, vec_list_size(polygon)-1);
  vector_t *first = vec_list_get(polygon, 0);
  sum += last->x * first->y - first->x * last->y;
  return sum/2;
}

// Returns a vector corresponding to the center of mass of the polygon
vector_t polygon_centroid(vec_list_t *polygon){
  double x = 0;
  double y = 0;
  for(size_t i = 0; i < vec_list_size(polygon)-1; ++i){
    vector_t *val_one = vec_list_get(polygon, i);
    vector_t *val_two = vec_list_get(polygon, i+1);
    double value = val_one->x * val_two->y -
                  val_two->x * val_one->y;
    x += (val_one->x + val_two->x) * value;
    y += (val_one->y + val_two->y) * value;
  }
  vector_t *first = vec_list_get(polygon, 0);
  vector_t *last = vec_list_get(polygon, vec_list_size(polygon)-1);
  double value = first->y * last->x - first->x * last->y;
  x += (first->x + last->x) * value;
  y += (first->y + last->y) * value;
  double val = AREAFACTOR * polygon_area(polygon);
  return vec_create(x/val, y/val);
}

// Translates the polygon by the vector translation
void polygon_translate(vec_list_t *polygon, vector_t translation){
  double x = translation.x;
  double y = translation.y;
  for(size_t i = 0; i < vec_list_size(polygon); ++i){
    vector_t *translate = vec_list_get(polygon, i);
    vector_t newpos = vec_create(translate->x + x, translate->y + y);
    *translate = newpos;
    // ((vector_t *) vec_list_get(polygon, i))->x += x;
    // ((vector_t *) vec_list_get(polygon, i))->y += y;
  }
}

// Rotates the vector by the given angle around the given point
void polygon_rotate(vec_list_t *polygon, double angle, vector_t point){
  polygon_translate(polygon, vec_negate(point));
  for(size_t i = 0; i < vec_list_size(polygon); ++i){
    vector_t *point = vec_list_get(polygon, i);
    *point = vec_rotate(*point, angle);
  }
  polygon_translate(polygon, point);
}
