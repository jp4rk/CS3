#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "body.h"
#include "scene.h"
#include "sdl_wrapper.h"

const size_t FRAME_WIDTH = 100;
const size_t STARTING_PELLETS = 10;
const double PELLET_SPAWN_RATE = 5;
const size_t PACMAN_RADIUS = 8;
const size_t PELLET_RADIUS = 1;
const size_t NUM_DEGREES = 360;
const size_t PACMAN_DEGREES = 300;
const size_t PELLET_MASS = 1;
const size_t PACMAN_MASS = 10;
const double FACE_RIGHT = 0;
const double FACE_UP = M_PI / 2;
const double FACE_LEFT = M_PI;
const double FACE_DOWN = 3 * M_PI / 2;
const double ACCELERATION = 20;

// This function returns a random float between 0 and 1.
float rand_float() {
    return (float)rand() / (float)RAND_MAX;
}

// This function creates the pacman body [centered at
// (FRAME_WIDTH / 2, FRAME_WIDTH / 4)]
body_t *create_pacman(rgb_color_t color) {
    list_t *points = list_init(PACMAN_DEGREES + 1, free);
    for (size_t i = 0; i < PACMAN_DEGREES; i++) {
        vector_t new_pt = {.x = PACMAN_RADIUS, .y = 0};

        // Rotate the point (one point for each degree to make 5/6 of a circle)
        int shift = (int)i;
        new_pt = vec_rotate(new_pt, shift * 2 * M_PI / NUM_DEGREES + M_PI / 6);

        // Allocate memory for this point
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = new_pt;
        list_add(points, new_vec);
    }

    // Add a single point at the center (where the mouth comes in)
    vector_t inner_pt = VEC_ZERO;
    vector_t *inner_vec = malloc(1 * sizeof(vector_t));
    assert(inner_vec != NULL);
    *inner_vec = inner_pt;
    list_add(points, inner_vec);

    body_t *pacman = body_init(points, PACMAN_MASS, color);

    vector_t center_scene = {.x = (float)FRAME_WIDTH / 2, .y = (float)FRAME_WIDTH / 4};
    body_set_centroid(pacman, center_scene);

    return pacman;
}

// This function creates a random centroid on the frame. It takes in a radius
// of the body, to make sure that the centroid is placed such that the entire
// body is in frame.
vector_t random_centroid(size_t radius) {
    vector_t result = {.x = rand_float() * (FRAME_WIDTH - radius),
                       .y = rand_float() * (((float)FRAME_WIDTH / 2) - radius)};
    return result;
}

// This function creates a generic pellet in a random location
body_t *create_pellet(rgb_color_t color) {
    list_t *points = list_init(NUM_DEGREES, free);
    for (size_t i = 0; i < NUM_DEGREES; i++) {
        vector_t new_pt = {.x = 0, .y = PELLET_RADIUS};

        // Rotate the point (one point for each degree to make a circle)
        int shift = (int)i;
        new_pt = vec_rotate(new_pt, shift * 2 * M_PI / NUM_DEGREES);

        // Allocate memory for this point
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = new_pt;
        list_add(points, new_vec);
    }
    body_t *pellet = body_init(points, PELLET_MASS, color);

    // Find random centroid and move pellet there
    vector_t new_centroid = random_centroid(PELLET_RADIUS);
    body_set_centroid(pellet, new_centroid);

    return pellet;
}

// This function checks if pacman has crossed any boundary, and if so,
// translates it through that wall to the other side.
void boundary_check(body_t *body) {
    list_t *shape = body_get_shape(body);
    vector_t centroid = body_get_centroid(body);
    for (size_t i = 0; i < list_size(shape); i++) {
        vector_t *current = list_get(shape, i);
        if (current->x < 0) {
            // Move body to the right side
            vector_t new_centroid = {.x = FRAME_WIDTH - PACMAN_RADIUS, .y = centroid.y};
            body_set_centroid(body, new_centroid);
            break;
        } else if (current->x > FRAME_WIDTH) {
            // Move body to the left side
            vector_t new_centroid = {.x = PACMAN_RADIUS, .y = centroid.y};
            body_set_centroid(body, new_centroid);
            break;
        } else if (current->y < 0) {
            // Move body to the top side
            vector_t new_centroid = {.x = centroid.x,
                                     .y = ((float)FRAME_WIDTH / 2) - PACMAN_RADIUS};
            body_set_centroid(body, new_centroid);
            break;
        } else if (current->y > (float)FRAME_WIDTH / 2) {
            // Move body to the bottom side
            vector_t new_centroid = {.x = centroid.x, .y = PACMAN_RADIUS};
            body_set_centroid(body, new_centroid);
            break;
        }
    }
    list_free(shape);
}

// This function checks if pellet has been "eaten" by pacman. This occurs
// when the pellet is entirely inside of pacman's circle. If a pellet has been
// eaten, return 1. Otherwise, return 0.
int got_eaten(body_t *pacman, body_t *pellet) {
    vector_t pacman_centroid = body_get_centroid(pacman);
    vector_t pellet_centroid = body_get_centroid(pellet);
    vector_t diff = vec_subtract(pacman_centroid, pellet_centroid);
    double distance = sqrt(diff.x * diff.x + diff.y * diff.y);
    if (distance <= PACMAN_RADIUS - PELLET_RADIUS) {
        return 1;
    }
    return 0;
}

// Key press handler
void on_key(char key, key_event_type_t type, double held_time, scene_t *scene) {
    body_t *pacman = scene_get_body(scene, 0);
    vector_t v = VEC_ZERO;
    if (type == KEY_PRESSED) {
        switch (key) {
            case LEFT_ARROW:
                body_set_rotation(pacman, FACE_LEFT);
                v.x = -1 * held_time * ACCELERATION;
                break;

            case RIGHT_ARROW:
                body_set_rotation(pacman, FACE_RIGHT);
                v.x = held_time * ACCELERATION;
                break;

            case UP_ARROW:
                body_set_rotation(pacman, FACE_UP);
                v.y = held_time * ACCELERATION;
                break;

            case DOWN_ARROW:
                body_set_rotation(pacman, FACE_DOWN);
                v.y = -1 * held_time * ACCELERATION;

                break;
        }
    }
    body_set_velocity(pacman, v);
}

scene_t *initialize_pacman_scene() {
    scene_t *scene = scene_init();

    rgb_color_t yellow = {.r = 1, .g = 1, .b = 0};

    // Create pacman and add him to the scene in index 0.
    body_t *pacman = create_pacman(yellow);
    scene_add_body(scene, pacman);

    // Add the starting pellets to the scene.
    for (size_t i = 0; i < STARTING_PELLETS; i++) {
        body_t *pellet = create_pellet(yellow);
        scene_add_body(scene, pellet);
    }
    return scene;
}

void check_eaten(scene_t *scene, body_t *pacman) {
    for (size_t j = 1; j < scene_bodies(scene); j++) {
        body_t *pellet = scene_get_body(scene, j);
        if (got_eaten(pacman, pellet)) {
            scene_remove_body(scene, j);
            j--;
        }
    }
}

int main() {
    // Create max and min vectors for frame
    vector_t max = {.x = FRAME_WIDTH, .y = (float)FRAME_WIDTH / 2};
    vector_t min = {.x = 0, .y = 0};

    sdl_init(min, max);

    // This value will represent the time since the last pellet was created.
    double pellet_time = 0;

    sdl_on_key(on_key);

    scene_t *scene = initialize_pacman_scene();

    while (!sdl_is_done(scene)) {
        body_t *pacman = scene_get_body(scene, 0);
        double dt = time_since_last_tick();

        scene_tick(scene, dt);
        boundary_check(pacman);

        // Check if any pellets have been eaten. Index 0 is pacman. If a
        // pellet has been eaten, remove it.
        check_eaten(scene, pacman);

        sdl_render_scene(scene);

        // Check if it is time to create a new pellet.
        if (pellet_time > PELLET_SPAWN_RATE) {
            rgb_color_t yellow = {.r = 1, .g = 1, .b = 0};
            body_t *pellet = create_pellet(yellow);
            scene_add_body(scene, pellet);
            pellet_time = 0;
        } else {
            pellet_time += dt;
        }
    }

    scene_free(scene);
    return 0;
}
