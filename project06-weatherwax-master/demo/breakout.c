#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "body.h"
#include "forces.h"
#include "scene.h"
#include "sdl_wrapper.h"

#define MAX ((vector_t) {.x = 100.0, .y = 100.0})

#define FRAME_WIDTH 50
#define DROP_Y 80
#define DELTA_X 10
#define FRAME_HEIGHT 80
#define BRICKS_PER_COLUMN 3
#define PLAYER_VELOCITY 90
#define CIRCLE_POINTS 60
#define BALL_RADIUS 1
#define BALL_MASS 5
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 4
#define WALL_WIDTH 200
#define WALL_HEIGHT 150
#define WALL_ELASTICITY 1
#define BRICKS_PER_ROW 10
#define BRICK_WIDTH 9
#define BRICK_HEIGHT 4
#define BOOST_OUTER_RADIUS ((vector_t) {.x = 0.0, .y = 2.0})
#define BOOST_INNER_RADIUS ((vector_t) {.x = 0.0, .y = 1.0})
#define BOOST_MASS 5
#define BOOST_FACTOR 1.25
#define START_VELOCITY ((vector_t) {.x = 35.0, .y = -45.0})

#define PADDLE_COLOR ((rgb_color_t) {.8, .2, .2})
#define BALL_COLOR ((rgb_color_t) {.6, .5, .2})
#define BOOST_COLOR ((rgb_color_t) {0, 0, 0})
#define WALL_COLOR ((rgb_color_t) {0, 0, 0})


typedef enum {
    BALL,
    PADDLE,
    BRICK,
    WALL,
    BOOST
} body_type_t;

body_type_t *make_type_info(body_type_t type) {
    body_type_t *info = malloc(sizeof(*info));
    *info = type;
    return info;
}

body_type_t get_type(body_t *body) {
    return *(body_type_t *) body_get_info(body);
}

double rand_double() {
    return (double)rand() / (double)RAND_MAX;
}

/** Constructs a rectangle with the given dimensions centered at (0, 0) */
list_t *rect_init(double width, double height) {
    vector_t half_width  = {.x = width / 2, .y = 0.0},
             half_height = {.x = 0.0, .y = height / 2};
    list_t *rect = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = vec_add(half_width, half_height);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_height, half_width);
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_negate(*(vector_t *) list_get(rect, 0));
    list_add(rect, v);
    v = malloc(sizeof(*v));
    *v = vec_subtract(half_width, half_height);
    list_add(rect, v);
    return rect;
}

/** Constructs a circles with the given radius centered at (0, 0) */
list_t *circle_init(double radius) {
    list_t *circle = list_init(CIRCLE_POINTS, free);
    double arc_angle = 2 * M_PI / CIRCLE_POINTS;
    vector_t point = {.x = radius, .y = 0.0};
    for (size_t i = 0; i < CIRCLE_POINTS; i++) {
        vector_t *v = malloc(sizeof(*v));
        *v = point;
        list_add(circle, v);
        point = vec_rotate(point, arc_angle);
    }
    return circle;
}

// Key press handler
void on_key(char key, key_event_type_t type, double held_time, void *aux) {
    scene_t *scene = (scene_t *) aux;
    body_t *paddle = scene_get_body(scene, 0);
    vector_t v = VEC_ZERO;
    if (type == KEY_PRESSED) {
        switch (key) {
            case LEFT_ARROW:
                v.x = -1 * PLAYER_VELOCITY;
                break;

            case RIGHT_ARROW:
                v.x = PLAYER_VELOCITY;
                break;

        }
    }
    body_set_velocity(paddle, v);
}

body_t *create_ball(vector_t center, vector_t velocity) {
  list_t *circle = circle_init(BALL_RADIUS);

  body_t *ball = body_init_with_info(
      circle,
      BALL_MASS,
      BALL_COLOR,
      make_type_info(BALL),
      free
  );

  body_set_velocity(ball, velocity);
  body_set_centroid(ball, center);

  return ball;

}

// This function adds the ball, and the accompanying forces.
// Must call this after you've added paddle, bricks, and walls!
void add_ball(scene_t *scene) {
    // This function creates a ball in a random spot, moving down
    vector_t ball_center = {
        .x = MAX.x / 2 + (rand_double() - 0.5) * DELTA_X,
        .y = DROP_Y
    };

    body_t * ball = create_ball(ball_center, START_VELOCITY);
    size_t body_count = scene_bodies(scene);
    scene_add_body(scene, ball);

    // Add force creators with other bodies
    for (size_t i = 0; i < body_count; i++) {
        body_t *body = scene_get_body(scene, i);
        switch (get_type(body)) {
            case BALL:
            case BOOST:
            case WALL:
                // Bounce off walls
                create_physics_collision(scene, WALL_ELASTICITY, ball, body);
                break;
            case PADDLE:
                // Bounce off the paddle
                create_physics_collision(scene, WALL_ELASTICITY, ball, body);
                break;
            case BRICK:
                // Bounce off bricks, while destroying them
                create_physics_destructive_collision(scene, WALL_ELASTICITY,
                    ball, body);
                break;
        }
    }
}

// Add the paddle but no forces
void add_paddle(scene_t *scene) {
    list_t *rectangle = rect_init(PADDLE_WIDTH, PADDLE_HEIGHT);

    body_t *paddle = body_init_with_info(
        rectangle,
        INFINITY,
        PADDLE_COLOR,
        make_type_info(PADDLE),
        free
    );

    vector_t paddle_center = (vector_t) {MAX.x / 2, PADDLE_HEIGHT / 2};
    body_set_centroid(paddle, paddle_center);

    scene_add_body(scene, paddle);
}

// Returns the rainbow color for the given index
rgb_color_t rainbow_color(size_t index) {
    rgb_color_t color = {0, 0, 0};
    if (index < 3) {
        color = (rgb_color_t) {
          .r = 1,
          .g = (double) index / 2,
          .b = 0
        };
    }
    else if (index < 6) {
        color = (rgb_color_t) {
            .r = 0,
            .g = (6 - (double) index) / 3,
            .b = ((double) index - 3) / 2
        };
    }
    else if (index < 10) {
        color = (rgb_color_t) {
            .r = ((double) index - 6) / 3,
            .g = 0,
            .b = (9 - (double) index) / 4
        };
    }
    return color;
}

// Adds the bricks but no forces
void add_bricks(scene_t *scene) {
    // There are BRICKS_PER_ROW + 1 gaps in a row
    double brick_gap = (double) (MAX.x - BRICKS_PER_ROW * BRICK_WIDTH) / (double) (BRICKS_PER_ROW + 1);
    for (size_t i = 0; i < BRICKS_PER_ROW; i++) {
        rgb_color_t color = rainbow_color(i);
        for (size_t j = 0; j < BRICKS_PER_COLUMN; j++) {
          vector_t center = {
              .x = (i + .5) * BRICK_WIDTH + (i + 1) * brick_gap,
              .y = MAX.y - ((j + .5) * BRICK_HEIGHT + (j + 1) * brick_gap)
          };
          list_t *rect = rect_init(BRICK_WIDTH, BRICK_HEIGHT);
          body_t *brick = body_init_with_info(rect, INFINITY, color,
              make_type_info(BRICK), free);
          body_set_centroid(brick, center);
          scene_add_body(scene, brick);
        }
    }
}

// Adds the walls on the left and right side but no forces
void add_walls(scene_t *scene) {
  // Add walls
  list_t *rect1 = rect_init(WALL_WIDTH, WALL_HEIGHT);
  body_t *left_wall = body_init_with_info(
      rect1,
      INFINITY,
      WALL_COLOR,
      make_type_info(WALL),
      free
  );

  vector_t left_center = {
      .x = -WALL_WIDTH / 2,
      .y = MAX.y / 2
  };

  body_set_centroid(left_wall, left_center);
  scene_add_body(scene, left_wall);

  list_t *rect2 = rect_init(WALL_WIDTH, WALL_HEIGHT);
  body_t *right_wall = body_init_with_info(
      rect2,
      INFINITY,
      WALL_COLOR,
      make_type_info(WALL),
      free
  );

  vector_t right_center = {
      .x = MAX.x + WALL_WIDTH / 2,
      .y = MAX.y / 2
  };

  body_set_centroid(right_wall, right_center);
  scene_add_body(scene, right_wall);
}

/*
 * This function creates a star (a body_t *) centered around the origin.
 * The function takes in a number of points for the star, which must be more
 * than 2.
 */
body_t* create_star(int num_points) {
    assert(num_points > 2);
    list_t* points = list_init(num_points, free);

    // Create the points
    for (size_t i = 0; i < num_points * 2; i++) {
        vector_t new_pt = VEC_ZERO;
        // Alternate between outer and inner points
        if (i % 2 == 0) {
            new_pt = vec_add(new_pt, BOOST_OUTER_RADIUS);
        }
        else {
            new_pt = vec_add(new_pt, BOOST_INNER_RADIUS);
        }

        // Rotate the point pi/num_points radians around (counterclockwise)
        // the center
        int shift = (int) i;
        new_pt = vec_rotate(new_pt, shift * M_PI / num_points);

        // Allocate memory for vector * to be returned
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = new_pt;
        list_add(points, new_vec);
    }
    body_t* star = body_init_with_info(points, BOOST_MASS, BOOST_COLOR,
        make_type_info(BOOST), free);

    return star;
}

// This collision boosts the first body by a boost factor, and removes the second
// body.
void boost_collision(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    vector_t current_velocity = body_get_velocity(body1);
    vector_t boosted_velocity = {
        .x = current_velocity.x * BOOST_FACTOR,
        .y = current_velocity.y * BOOST_FACTOR
    };
    body_set_velocity(body1, boosted_velocity);
    body_remove(body2);
}

// Power up function. This adds a force between the boost and the ball.
// The force will destroy the boost, and speed up the ball.
void add_speed_boost(scene_t *scene) {
    body_t *boost = create_star(5);

    // The boost can be anywhere on the frame horizontally, but it will not
    // be in the top or bottom quarters, to avoid hitting paddle/bricks
    vector_t boost_center = {
        .x = rand_double() * (MAX.x - BOOST_OUTER_RADIUS.y),
        .y = rand_double() * (MAX.y / 2) + MAX.y / 4
    };

    body_set_centroid(boost, boost_center);

    size_t body_count = scene_bodies(scene);
    scene_add_body(scene, boost);

    // Add force creators with other bodies
    for (size_t i = 0; i < body_count; i++) {
        body_t *body = scene_get_body(scene, i);
        if (get_type(body) == BALL) {
            // Create boost for ball
            create_collision(scene, body, boost,
                (collision_handler_t)boost_collision, NULL, NULL);
        }
    }
}

// This body takes in the scene, and checks if the ball has passed through
// the top or bottom of the screen.
bool check_end(scene_t *scene) {
    bool above = true;
    bool below = true;
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        if (get_type(body) == BALL) {
            body_t *ball = body;
            list_t *shape = body_get_shape(ball);
            for (size_t i = 0; i < list_size(shape); i++) {
                vector_t *current = list_get(shape, i);
                if (current->y < MAX.y) {
                    above = false;
                }
                if (current->y > 0) {
                    below = false;
                }
            }
            list_free(shape);
            break;
        }
    }
    return (above || below);
}

scene_t *reset() {
  scene_t *scene = scene_init();
  add_paddle(scene);
  add_bricks(scene);
  add_walls(scene);
  add_ball(scene);
  return scene;
}

void check_walls(scene_t *scene) {
    body_t *paddle = scene_get_body(scene, 0);
    list_t *shape = body_get_shape(paddle);
    for (size_t i = 0; i < list_size(shape); i++) {
        vector_t *current = list_get(shape, i);
        if (current->x < 0) {
            vector_t center = {.x = PADDLE_WIDTH / 2, .y = PADDLE_HEIGHT / 2};
            body_set_centroid(paddle, center);
            break;
        }
        else if (current->x > MAX.x) {
            vector_t center =
                {.x = MAX.x - PADDLE_WIDTH / 2, .y = PADDLE_HEIGHT / 2};
            body_set_centroid(paddle, center);
            break;
        }
    }
    list_free(shape);
}


int main() {
    sdl_init(VEC_ZERO, MAX);

    scene_t *scene = reset();

    sdl_on_key(on_key);

    double last_speed_boost = 0;

    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);

        check_walls(scene);

        sdl_render_scene(scene);

        last_speed_boost += dt;
        if (last_speed_boost > 7) {
            last_speed_boost = 0;
            add_speed_boost(scene);
        }

        if (check_end(scene)) {
            scene_free(scene);
            scene = reset();
            last_speed_boost = 0;
        }
    }

    scene_free(scene);
    return 0;
}
