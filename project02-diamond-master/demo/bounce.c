#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "sdl_wrapper.h"
#include "polygon.h"
#include "vector.h"
#include "shape.h"
#include "vec_list.h"
#include <SDL2/SDL.h>

const double ROT_PER_SEC = 0.25;
const double OUTER_RADIUS = 50;
const double INNER_RADIUS = 30;
const int NUM_SIDES = 4;
const vector_t MIN = {0,0};
const vector_t MAX = {1000, 500};
const vector_t INIT_VELOCITY = {150, -100};
const vector_t CENTER = {0, 500};
const vector_t GRAVITY = {0, 1};
const double ELASTICITY = 0.8;
const int MAX_NUM_ON_SCREEN = 100;
const int MAX_SIDES = 17;
const int MIN_SIDES = 4;

// Returns a shape with the following properties
shape_t *shape_init(int num_sides, double r, double g, double b){
  // Initializing the star with the given features
  shape_t *star = malloc(sizeof(shape_t));
  star->r = r;
  star->g = g;
  star->b = b;
  star->points = malloc(sizeof(vec_list_t *));
  *(star->points) = vec_list_init(MAX_SIDES * 2);
  star->outer_radius = OUTER_RADIUS;
  star->inner_radius = INNER_RADIUS;
  star->velocity = malloc(sizeof(vector_t));
  star->velocity->x = INIT_VELOCITY.x;
  star->velocity->y = INIT_VELOCITY.y;

  // Star properties
  double angle_rotate = M_PI / (num_sides);
  vector_t top_point = vec_create(CENTER.x, CENTER.y + OUTER_RADIUS);
  vector_t inner_point = vec_rotate(vec_subtract(top_point, CENTER), angle_rotate);
  double factor = INNER_RADIUS/OUTER_RADIUS;
  inner_point = vec_multiply(factor, inner_point);
  inner_point = vec_add(inner_point, CENTER);

  // Adds all points to star
  for(int i = 0; i < num_sides; ++i){
    vector_t *next_outer = malloc(sizeof(vector_t));
    *next_outer = vec_rotate(vec_subtract(top_point, CENTER), 2*i*angle_rotate);
    *next_outer = vec_add(*next_outer, CENTER);
    vec_list_add(*(star->points), next_outer);

    vector_t *next_inner = malloc(sizeof(vector_t));
    *next_inner = vec_rotate(vec_subtract(inner_point, CENTER), 2*i*angle_rotate);
    *next_inner = vec_add(*next_inner, CENTER);
    vec_list_add(*(star->points), next_inner);
  }
  return star;
}

// Initializes a shape with random colors and number of sides.
// The velocity and two radii are constant.
shape_t *shape_rand_init(){
  int sides = MIN_SIDES + rand()%(MAX_SIDES-MIN_SIDES);
  int r;
  int g;
  int b;
  srand(time(NULL));
  r = rand()%100;
  g = rand()%100;
  b = rand()%100;
  shape_t *ans = shape_init(sides, (double) r/100, (double) g/100, (double) b/100);
  return ans;
}

// Updates a single shape
bool shape_update(shape_t *shape, double time){
  bool ans = false;
  vector_t velocity = *shape_get_velocity(shape);
  vector_t change_position = vec_multiply(time, velocity);
  vector_t centroid = polygon_centroid(shape_get_points(shape));
  double changex = 0;
  double changey = 0;
  bool over = false;

  // Tests if the position of the x coordinate is out of bounds
  if(change_position.x > 0){
    double val = centroid.x - OUTER_RADIUS + change_position.x;
    if(val >= MAX.x){
      ans = true;
    }
  }

  // Tests if the position of the y coordinate is out of bounds
  if(change_position.y < 0){
    if(centroid.y - OUTER_RADIUS - change_position.y <= MIN.y){
      velocity.y *= -1*ELASTICITY;
      changey = MIN.y + centroid.y - OUTER_RADIUS - change_position.y;
      over = true;
    }
  }

  // If over, correct the movement
  if(over){
    vector_t newcoords = vec_create(changex, changey);
    polygon_translate(shape_get_points(shape), vec_subtract(change_position, newcoords));
  }
  else {
    polygon_translate(shape_get_points(shape), change_position);
  }
  velocity.y -= GRAVITY.y;
  *shape->velocity = velocity;
  polygon_rotate(shape_get_points(shape), time * ROT_PER_SEC * 2 * M_PI, centroid);
  return ans;
}

int main(){
  sdl_init(MIN, MAX);
  vec_list_t *store_shapes = vec_list_init(MAX_NUM_ON_SCREEN);
  shape_t *first = shape_rand_init();
  vec_list_add(store_shapes, first);
  // The interval between spawning shapes
  double interval = 1.5;
  double track_time = 0;

  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    sdl_clear();
    track_time += dt;

    // If the interval is reached, spawns another shape
    if(track_time >= interval){
      shape_t *new_shape = shape_rand_init();
      vec_list_add(store_shapes, new_shape);
      track_time = 0;
    }

    for(size_t i = 0; i < vec_list_size(store_shapes); i++) {
      // Removes a shape that is off screen if it exists
      if(shape_update(vec_list_get(store_shapes, i), dt)){
        shape_t *ans = vec_list_remove_index(store_shapes, 0);
        shape_free(ans);
        i--;
      }
      else{
        sdl_draw_polygon(shape_get_points(vec_list_get(store_shapes, i)), shape_get_r(vec_list_get(store_shapes, i)),
        shape_get_g(vec_list_get(store_shapes, i)), shape_get_b(vec_list_get(store_shapes, i)));
      }
    }
    sdl_show();
  }

  // Frees remaining shapes in the list
  for(size_t i = 0; i < vec_list_size(store_shapes); ++i){
    shape_t *ans = vec_list_get(store_shapes, i);
    shape_free(ans);
  }
  free(store_shapes);
  return 0;
}
