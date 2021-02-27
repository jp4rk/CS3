#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "body.h"
#include "forces.h"
#include "scene.h"
#include "sdl_wrapper.h"

const size_t FRAME_WIDTH = 50;
const size_t FRAME_HEIGHT = 80;
const double GRAVITY = 25;
const double MAX_RADIUS = 6;
const size_t NUM_DEGREES = 360;
const size_t ENEMIES_PER_COLUMN = 3;
const size_t ENEMY_RADIUS = 4;
const size_t ENEMY_MASS = 5;
const int ENEMY = 1;
const int PLAYER = 0;
const size_t PLAYER_MASS = 5;
const size_t PLAYER_VERT_RADIUS = 2;
const size_t PLAYER_HORIZ_RADIUS = 4;
const double PLAYER_VELOCITY = 40;
const double ENEMY_VELOCITY = 10;
const double PROJECTILE_VELOCITY = 40;
const size_t PROJECTILE_MASS = 1;
const size_t HORIZONTAL_GAP = 1;
const size_t VERTICAL_GAP = 1;
const size_t RECTANGLE_SIZE = 1;

typedef struct team_info {
    int team;
} team_info_t;

double rand_float() {
    return (double)rand() / (double)RAND_MAX;
}

// This function creates an enemy body, centered at (0,0)
body_t *create_enemy(rgb_color_t color) {
    list_t *points = list_init(NUM_DEGREES / 2 + 1, free);
    for (size_t i = 0; i < NUM_DEGREES / 2; i++) {
        vector_t new_pt = {.x = ENEMY_RADIUS, .y = 0};

        // Rotate the point (one point for each degree to make a circle)
        int shift = (int)i;
        new_pt = vec_rotate(new_pt, shift * 2 * M_PI / NUM_DEGREES);

        // Allocate memory for this point
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = new_pt;
        list_add(points, new_vec);
    }
    vector_t final_pt = {.x = 0, .y = -1 * (double) ENEMY_RADIUS / 2};
    vector_t *new_vec = malloc(1 * sizeof(vector_t));
    assert(new_vec != NULL);
    *new_vec = final_pt;
    list_add(points, new_vec);

    team_info_t *info = malloc(1 * sizeof(team_info_t));
    info->team = ENEMY;

    body_t *enemy = body_init_with_info(points, ENEMY_MASS, color, info, free);
    vector_t velo = {.x = ENEMY_VELOCITY, .y = 0};
    body_set_velocity(enemy, velo);

    return enemy;
}

// This function creates the player body, centered at (.5, .05) -> relative
// to frame width
body_t *create_player(rgb_color_t color) {
    list_t *points = list_init(NUM_DEGREES, free);
    for (size_t i = 0; i < NUM_DEGREES; i++) {
        int shift = (int)i;
        vector_t new_pt = {.x = PLAYER_HORIZ_RADIUS * cos(shift * 2 * M_PI / NUM_DEGREES),
           .y = PLAYER_VERT_RADIUS * sin(shift * 2 * M_PI / NUM_DEGREES)};

        // Allocate memory for this point
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = new_pt;
        list_add(points, new_vec);
    }

    team_info_t *info = malloc(1 * sizeof(team_info_t));
    info->team = PLAYER;

    body_t *player = body_init_with_info(points, PLAYER_MASS, color, info, free);
    vector_t player_centroid = {
        .x = FRAME_WIDTH / 2,
        .y = FRAME_HEIGHT / 10
    };
    body_set_centroid(player, player_centroid);

    return player;
}


// Add collision force to everything else in the scene on other team
void add_body_forces(scene_t * scene, body_t *new_body) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        team_info_t *info1 = (team_info_t *) body_get_info(scene_get_body(scene, i));
        team_info_t *info2 = (team_info_t *) body_get_info(new_body);
        if (info1->team != info2->team) {
            create_destructive_collision(scene, scene_get_body(scene, i), new_body);
        }
    }
}

list_t* make_rectangle(){
    vector_t point1 = {.x = 0, .y = 1};
    vector_t *vec1 = malloc(1 * sizeof(vector_t));
    *vec1 = point1;
    vector_t point2 = {.x = 0, .y = 0};
    vector_t *vec2 = malloc(1 * sizeof(vector_t));
    *vec2 = point2;
    vector_t point3 = {.x = .5, .y = 0};
    vector_t *vec3 = malloc(1 * sizeof(vector_t));
    *vec3 = point3;
    vector_t point4 = {.x = .5, .y = 1};
    vector_t *vec4 = malloc(1 * sizeof(vector_t));
    *vec4 = point4;
    list_t * rectangle = list_init(4, free);
    list_add(rectangle, vec1);
    list_add(rectangle, vec2);
    list_add(rectangle, vec3);
    list_add(rectangle, vec4);
    return rectangle;
}

body_t *create_projectile(body_t *body) {
  list_t* rectangle = make_rectangle();
  team_info_t *info = (team_info_t *) body_get_info(body);
  team_info_t *new_info = malloc(sizeof(team_info_t));
  *new_info = *info;
  if (info->team == ENEMY) {
      body_t * projectile = body_init_with_info(rectangle, PROJECTILE_MASS,
          body_get_color(body), new_info, body_get_info_freer(body));
      vector_t velo = {.x = 0, .y = -1 * PROJECTILE_VELOCITY};
      body_set_velocity(projectile, velo);
      vector_t body_centroid = body_get_centroid(body);
      vector_t projectile_centroid = {.x = body_centroid.x,
          .y = body_centroid.y - .75 * ENEMY_RADIUS - RECTANGLE_SIZE / 2};
      body_set_centroid(projectile, projectile_centroid);
      return projectile;
  }
  else {
      body_t * projectile = body_init_with_info(rectangle, PROJECTILE_MASS,
          body_get_color(body), new_info, body_get_info_freer(body));
      vector_t velo = {.x = 0, .y = PROJECTILE_VELOCITY};
      body_set_velocity(projectile, velo);
      vector_t body_centroid = body_get_centroid(body);
      vector_t projectile_centroid = {.x = body_centroid.x,
          .y = body_centroid.y + PLAYER_VERT_RADIUS + RECTANGLE_SIZE / 2};
      body_set_centroid(projectile, projectile_centroid);
      return projectile;
  }
}


// Key press handler
void on_key(char key, key_event_type_t type, double held_time, void *aux) {
    scene_t *scene = (scene_t *) aux;
    body_t *player = scene_get_body(scene, 0);
    vector_t v = VEC_ZERO;
    if (type == KEY_PRESSED) {
        switch (key) {
            case LEFT_ARROW:
                v.x = -1 * PLAYER_VELOCITY;
                break;

            case RIGHT_ARROW:
                v.x = PLAYER_VELOCITY;
                break;

            case SPACEBAR:
            {
                body_t *projectile = create_projectile(player);
                add_body_forces(scene, projectile);
                scene_add_body(scene, projectile);
                break;
            }
        }
    }
    body_set_velocity(player, v);
}

void horiz_boundary_check(body_t *body) {
    // The enemies get shifted down and change directions when they hit a wall.
    list_t * shape = body_get_shape(body);
    team_info_t *info = (team_info_t *) body_get_info(body);
    if (info->team == ENEMY) {
        for (size_t i = 0; i < list_size(shape); i++) {
            vector_t * current = list_get(shape, i);
            if (current->x > FRAME_WIDTH) {
                vector_t centroid = {
                  .x = FRAME_WIDTH - ENEMY_RADIUS,
                  .y = body_get_centroid(body).y - 3 * VERTICAL_GAP - 4.5 * ENEMY_RADIUS
                };
                body_set_centroid(body, centroid);
                body_set_velocity(body, vec_multiply(-1, (body_get_velocity(body))));
                break;
            }
            else if (current->x < 0) {
                vector_t centroid = {
                  .x = ENEMY_RADIUS,
                  .y = body_get_centroid(body).y - 3 * VERTICAL_GAP - 4.5 * ENEMY_RADIUS
                };
                body_set_centroid(body, centroid);
                body_set_velocity(body, vec_multiply(-1, (body_get_velocity(body))));
                break;
            }
        }
    }
    // The player cannot move through the right or left walls.
    else {
        for (size_t i = 0; i < list_size(shape); i++) {
            vector_t * current = list_get(shape, i);
            if (current->x > FRAME_WIDTH) {
                vector_t centroid = {
                  .x = FRAME_WIDTH - PLAYER_HORIZ_RADIUS,
                  .y = body_get_centroid(body).y
                };
                body_set_centroid(body, centroid);
                break;
            }
            else if (current->x < 0) {
                vector_t centroid = {
                  .x = PLAYER_HORIZ_RADIUS,
                  .y = body_get_centroid(body).y
                };
                body_set_centroid(body, centroid);
                break;
            }
        }
    }
    list_free(shape);
}

void vert_boundary_check(body_t *body) {
    vector_t centroid = body_get_centroid(body);
    if (centroid.y > FRAME_HEIGHT || centroid.y < 0) {
        body_remove(body);
    }
}

int main() {
    // Create max and min vectors for frame
    vector_t max = {.x = FRAME_WIDTH, .y = FRAME_HEIGHT};
    vector_t min = {.x = 0, .y = 0};

    sdl_init(min, max);

    scene_t *scene = scene_init();

    rgb_color_t enemy_color = {
        .r = .4,
        .g = .4,
        .b = .4
    };

    rgb_color_t player_color = {
        .r = 0,
        .g = .9,
        .b = 0
    };

    body_t *player = create_player(player_color);
    scene_add_body(scene, player);

    size_t num_enemies = (FRAME_WIDTH / (2 * ENEMY_RADIUS + HORIZONTAL_GAP) - 1) * ENEMIES_PER_COLUMN;
    double y_pos = (double) FRAME_HEIGHT - (double) FRAME_HEIGHT / 20;
    double x_pos = (double) ENEMY_RADIUS + HORIZONTAL_GAP;
    size_t row_enemies = 0;
    for (size_t i = 0; i < num_enemies; i++) {
        body_t *new_enemy = create_enemy(enemy_color);
        vector_t centroid = {.x = x_pos, .y = y_pos};
        body_set_centroid(new_enemy, centroid);
        x_pos += 2 * (double) ENEMY_RADIUS + (double) HORIZONTAL_GAP;
        row_enemies += 1;
        if (row_enemies == num_enemies / (double) ENEMIES_PER_COLUMN) {
            row_enemies = 0;
            y_pos -= (1.5 * (double) ENEMY_RADIUS + (double) VERTICAL_GAP);
            x_pos = (double) ENEMY_RADIUS + HORIZONTAL_GAP;
        }
        add_body_forces(scene, new_enemy);
        scene_add_body(scene, new_enemy);
    }

    sdl_on_key(on_key);

    double last_projectile = 0;

    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        for (size_t i = 0; i < scene_bodies(scene); i++) {
            horiz_boundary_check(scene_get_body(scene, i));
            vert_boundary_check(scene_get_body(scene, i));
        }
        sdl_render_scene(scene);


        if (scene_bodies(scene) <= 1) {
            break;
        }
        team_info_t *player_info = body_get_info(scene_get_body(scene, 0));
        if (player_info->team != PLAYER) {
            break;
        }

        if (last_projectile >= 1) {
            num_enemies = 0;
            for (size_t i = 0; i < scene_bodies(scene); i++) {
                body_t *current = scene_get_body(scene, i);
                team_info_t *info = body_get_info(current);
                if (info->team == ENEMY) {
                    num_enemies += 1;
                }
            }
            double rand = rand_float();
            double selection = (double) num_enemies * rand;
            int rand_enemy = (int) selection + 1;
            body_t *projectile = create_projectile(scene_get_body(scene, rand_enemy));
            add_body_forces(scene, projectile);
            scene_add_body(scene, projectile);
            last_projectile = 0;
        }
        else {
            last_projectile += dt;
        }
    }

    scene_free(scene);
    return 0;
}
