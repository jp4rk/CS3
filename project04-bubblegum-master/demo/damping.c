#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "body.h"
#include "forces.h"
#include "scene.h"
#include "sdl_wrapper.h"

const size_t FRAME_WIDTH = 100;
const size_t PELLET_RADIUS = 2;
const double K = 2;
const double GAMMA = .05;
const size_t NUM_DEGREES = 360;
const size_t PELLET_MASS = 5;
const size_t ANCHOR_MASS = 10000;

// This function returns a random float between 0 and 1.
float rand_float() {
    return (float)rand() / (float)RAND_MAX;
}

list_t *make_shape() {
    list_t *shape = list_init(4, free);
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){-1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){+1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){+1, +1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){-1, +1};
    list_add(shape, v);
    return shape;
}

// This function creates a generic pellet in a random location
body_t *create_pellet(rgb_color_t color, vector_t centroid) {
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

    body_set_centroid(pellet, centroid);

    return pellet;
}

body_t *create_anchor(vector_t centroid, rgb_color_t color) {
    list_t *points = make_shape();
    body_t *anchor = body_init(points, ANCHOR_MASS, color);
    body_set_centroid(anchor, centroid);
    return anchor;
}

int main() {
    // Create max and min vectors for frame
    vector_t max = {.x = FRAME_WIDTH, .y = (float)FRAME_WIDTH / (float)2};
    vector_t min = {.x = 0, .y = 0};

    sdl_init(min, max);

    size_t n = FRAME_WIDTH / (PELLET_RADIUS * 2);

    scene_t *scene = scene_init();

    for (size_t i = 0; i < n; i++) {
        rgb_color_t white = {.r = 1, .g = 1, .b = 1};
        vector_t centroid = {.x = PELLET_RADIUS * (2 * i + 1),
                             .y = (float)FRAME_WIDTH / (float)4};
        body_t *anchor = create_anchor(centroid, white);
        scene_add_body(scene, anchor);
    }

    for (size_t i = 0; i < n; i++) {
        rgb_color_t color = {
            .r = (float)i / (float)n, .g = (float)i / (float)n, .b = (float)i / (float)n};
        if (i < n / 2) {
            vector_t centroid = {
                .x = PELLET_RADIUS * (2 * i + 1),
                .y =
                    (float)FRAME_WIDTH / 2 * (1 - ((float)i / (float)n)) - PELLET_RADIUS};
            body_t *new_pellet = create_pellet(color, centroid);
            scene_add_body(scene, new_pellet);
        } else {
            vector_t centroid = {.x = PELLET_RADIUS * (2 * i + 1),
                                 .y = (float)FRAME_WIDTH / 4};
            body_t *new_pellet = create_pellet(color, centroid);
            scene_add_body(scene, new_pellet);
        }
    }

    // Add the desired forces
    for (size_t i = 0; i < n; i++) {
        create_spring(scene, K, scene_get_body(scene, i), scene_get_body(scene, i + n));
        create_drag(scene, GAMMA, scene_get_body(scene, i));
    }

    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
    return 0;
}
