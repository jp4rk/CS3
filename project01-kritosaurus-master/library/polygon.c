#include "polygon.h"

vector_t create_vector(double x, double y);

/**
 * Computes the area of a polygon.
 * See https://en.wikipedia.org/wiki/Shoelace_formula#Statement.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the area of the polygon
 */
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

/**
 * Computes the center of mass of a polygon.
 * See https://en.wikipedia.org/wiki/Centroid#Of_a_polygon.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the centroid of the polygon
 */
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
  double val = 6 * polygon_area(polygon);
  return create_vector(x/val, y/val);
}

/**
 * Translates all vertices in a polygon by a given vector.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param translation the vector to add to each vertex's position
 */
void polygon_translate(vec_list_t *polygon, vector_t translation){
  double x = translation.x;
  double y = translation.y;
  for(size_t i = 0; i < vec_list_size(polygon); ++i){
    vec_list_get(polygon, i)->x += x;
    vec_list_get(polygon, i)->y += y;
  }
}

/**
 * Rotates vertices in a polygon by a given angle about a given point.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param angle the angle to rotate the polygon, in radians.
 * A positive angle means counterclockwise.
 * @param point the point to rotate around
 */
void polygon_rotate(vec_list_t *polygon, double angle, vector_t point){
  polygon_translate(polygon, vec_negate(point));
  for(size_t i = 0; i < vec_list_size(polygon); ++i){
    vector_t *point = vec_list_get(polygon, i);
    *point = vec_rotate(*point, angle);
  }
  polygon_translate(polygon, point);
}
