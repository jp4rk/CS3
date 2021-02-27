#include "vector.h"
#include <math.h>

const vector_t VEC_ZERO = {0,0};

// Returns a vector with the specified x and y vales
vector_t vec_create(double x, double y){
  vector_t ans;
  ans.x = x;
  ans.y = y;
  return ans;
}

// Returns the sum of v1 and v2
vector_t vec_add(vector_t v1, vector_t v2) {
  return vec_create(v1.x + v2.x, v1.y + v2.y);
}

// Returns the difference of v1 and v2
vector_t vec_subtract(vector_t v1, vector_t v2){
  return vec_create(v1.x - v2.x, v1.y - v2.y);
}

// Returns the additive inverse of v
vector_t vec_negate(vector_t v){
  return vec_create(v.x * -1, v.y * -1);
}

// Returns a vector scaled by scalar
vector_t vec_multiply(double scalar, vector_t v){
  return vec_create(v.x * scalar, v.y * scalar);
}

// Returns the dot product of v1 and v2
double vec_dot(vector_t v1, vector_t v2){
  return v1.x * v2.x + v1.y * v2.y;
}

// Returns the cross product of v1 and v2
double vec_cross(vector_t v1, vector_t v2){
  return v1.x * v2.y - v1.y * v2.x;
}

// Returns a vector rotated by angle about (0, 0)
vector_t vec_rotate(vector_t v, double angle){
  double newx = v.x * cos(angle) - v.y * sin(angle);
  double newy = v.x * sin(angle) + v.y * cos(angle);
  return vec_create(newx, newy);
}
