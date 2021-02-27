#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "body.h"
#include "forces.h"
#include "scene.h"
#include "sdl_wrapper.h"

const size_t FRAME_WIDTH = 100;
const double GRAVITY = 25;
const double MAX_RADIUS = 6;

// This function returns a random float between 0 and 1.
float rand_float() {
    return (float)rand() / (float)RAND_MAX;
}

// This function creates a random centroid on the frame. It takes in a radius
// of the body, to make sure that the centroid is placed such that the entire
// body is in frame.
vector_t random_centroid(size_t radius) {
    vector_t result = {.x = rand_float() * (FRAME_WIDTH - radius),
                       .y = rand_float() * (((float)FRAME_WIDTH / 2) - radius)};
    return result;
}

/*
 * This function creates a star (a body_t *) centered around the origin.
 * The function takes in a number of points for the star, which must be more
 * than 2. The star is a random color.
 */
body_t *create_star(int num_points) {
    assert(num_points > 2);

    list_t *points = list_init(num_points * 2, free);

    double radius = rand_float() * MAX_RADIUS;

    // Length of outer points set here
    vector_t outer = {.x = 0.0, .y = radius};

    // Length of inner points set here
    vector_t inner = {.x = 0.0, .y = radius / 2};

    // Create the points
    for (size_t i = 0; i < num_points * 2; i++) {
        vector_t new_pt = VEC_ZERO;
        // Alternate between outer and inner points
        if (i % 2 == 0) {
            new_pt = vec_add(new_pt, outer);
        } else {
            new_pt = vec_add(new_pt, inner);
        }

        // Rotate the point pi/num_points radians around (counterclockwise)
        // the center
        int shift = (int)i;
        new_pt = vec_rotate(new_pt, shift * M_PI / num_points);

        // Allocate memory for vector * to be returned
        vector_t *new_vec = malloc(1 * sizeof(vector_t));
        assert(new_vec != NULL);
        *new_vec = new_pt;
        list_add(points, new_vec);
    }

    rgb_color_t color = {.r = rand_float(), .g = rand_float(), .b = rand_float()};
    body_t *star = body_init(points, radius * 2, color);

    vector_t position = random_centroid(radius);
    body_set_centroid(star, position);

    return star;
}

void usage(char *program) {
    printf("Usage: %s n (Where n is the number of bodies)\n", program);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    // Create max and min vectors for frame
    vector_t max = {.x = FRAME_WIDTH, .y = (float)FRAME_WIDTH / 2};
    vector_t min = {.x = 0, .y = 0};

    sdl_init(min, max);

    scene_t *scene = scene_init();

    // Add n stars to the scene
    for (size_t i = 0; i < n; i++) {
        body_t *new_star = create_star(4);
        scene_add_body(scene, new_star);
    }

    // Add the desired forces
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            create_newtonian_gravity(scene, GRAVITY, scene_get_body(scene, i),
                                     scene_get_body(scene, j));
        }
    }

    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    scene_free(scene);
    return 0;
}
