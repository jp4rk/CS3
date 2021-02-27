#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <stdio.h>
#include "sdl_wrapper.h"
#include "polygon.h"
#include "vector.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 500

const double ROT_PER_SEC = 0.25;
const int RADIUS = 50;
const int NUM_SIDES = 5;
const vector_t MIN = {0,0};
const vector_t MAX = {1000, 500};

// Creates a star with a given radius and center
vec_list_t *make_star(int radius, int num_sides, vector_t center){
  vec_list_t *star = vec_list_init(2 * num_sides);
  vector_t top_point = create_vector(center.x, center.y + radius);

  // Trig to calculate the inner points of the star
  double inner_angle = M_PI * (num_sides - 2) / num_sides;
  double inner_radius = RADIUS / (sin(inner_angle/2) *
                        (1+tan(M_PI/2-inner_angle/2)*tan(M_PI-inner_angle)));
  vector_t inner_point = vec_rotate(vec_subtract(top_point, center), 7*(M_PI/2 - inner_angle/2));
  inner_point = vec_multiply(inner_radius/RADIUS, inner_point);
  inner_point = vec_add(inner_point, center);

  // Adds all points to star
  for(int i = 0; i < num_sides; ++i){
    double angle_rotate = i * M_PI * (num_sides-1)/(num_sides);
    vector_t *next_outer = malloc(sizeof(vector_t));
    *next_outer = vec_rotate(vec_subtract(top_point, center), angle_rotate);
    *next_outer = vec_add(*next_outer, center);
    vec_list_add(star, next_outer);

    vector_t *next_inner = malloc(sizeof(vector_t));
    *next_inner = vec_rotate(vec_subtract(inner_point, center), angle_rotate);
    *next_inner = vec_add(*next_inner, center);
    vec_list_add(star, next_inner);
  }

  return star;
}

/*
Updates the velocity and the position of the star
Changes the direction of the velocity and displacement if the star hits the wall
Rotates by a constant amount
*/
vector_t update(vector_t velocity, double time, vec_list_t *polygon, double radius, vector_t win_min, vector_t win_max){
  vector_t centroid = polygon_centroid(polygon);

  // How far the ball wants to travel
  vector_t change_position = vec_multiply(time, velocity);
  double changex = 0;
  double changey = 0;
  bool over = false;

  // Tests if the position of the x coordinate is out of bounds
  if(change_position.x > 0){
    if(centroid.x + radius + change_position.x >= win_max.x){
      velocity.x *= -1;
      changex = centroid.x + radius + change_position.x - win_max.x;
      over = true;
    }
  }
  else{
    if(centroid.x - radius - change_position.x <= win_min.x){
      velocity.x *= -1;
      changex = win_min.x + centroid.x - radius - change_position.x;
      over = true;
    }
  }

  // Tests if the position of the y coordinate is out of bounds
  if(change_position.y > 0){
    if(centroid.y + radius + change_position.y >= win_max.y){
      velocity.y *= -1;
      changey = centroid.y + radius + change_position.y - win_max.y;
      over = true;
    }
  }
  else{
    if(centroid.y - radius - change_position.y <= win_min.y){
      velocity.y *= -1;
      changey = win_min.y + centroid.y - radius - change_position.y;
      over = true;
    }
  }

  // If out of bounds, correct the movement
  if(over){
    vector_t newcoords = create_vector(changex, changey);
    polygon_translate(polygon, vec_subtract(change_position, newcoords));
  }
  else{
    polygon_translate(polygon, change_position);
  }

  // Rotates the star by the appropriate amount of radians per second (constant)
  polygon_rotate(polygon, time * ROT_PER_SEC * 2 * M_PI, centroid);
  return velocity;
}


int main(){
  vector_t velocity = create_vector(100, 100);
  vector_t center = create_vector(800, 250);
  vec_list_t *star = make_star(RADIUS, NUM_SIDES, center);
  sdl_init(MIN, MAX);
  while(!sdl_is_done()){
    double dt = time_since_last_tick();
    sdl_clear();
    velocity = update(velocity, dt, star, RADIUS, MIN, MAX);
    sdl_draw_polygon(star, 0, 1, 0);
    sdl_show();
  }
  vec_list_free(star);
  return 0;
}
