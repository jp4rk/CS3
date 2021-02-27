#include "sdl_wrapper.h"
#include "polygon.h"
#include "body.h"
#include <assert.h>
#include <stdio.h>

// Initialize constants
const double ROTATION_FACTOR = M_PI/720;
const float ELASTICITY = .75;
const float ACCELERATION = 75;
const int FRAME_WIDTH = 100;
const float RADIUS = 8.0;

/*
 * This function creates a star (a body_t *) centered around the origin.
 * The function takes in a number of points for the star, which must be more
 * than 2.
 */
body_t* create_star(int num_points, float r, float g, float b, int width) {
    assert(num_points > 2);

    list_t* points = list_init(num_points, free);

    // Length of outer points set here
    vector_t outer = {
        .x = 0.0,
        .y = 8.0
    };

    // Length of inner points set here
    vector_t inner = {
        .x = 0.0,
        .y = 3.5
    };

    // Create the points
    for (size_t i = 0; i < num_points * 2; i++) {
        vector_t new_pt = VEC_ZERO;
        // Alternate between outer and inner points
        if (i % 2 == 0) {
            new_pt = vec_add(new_pt, outer);
        }
        else {
            new_pt = vec_add(new_pt, inner);
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

    rgb_color_t color = {
        .r = r,
        .g = g,
        .b = b
    };
    body_t* star = body_init(points, 10, color);

    return star;
}

// Elasticity is (0,1) and represents the percentage of vertical velocity
// magnitude maintained in a collision.
void elastic_bounce(body_t *shape, float elasticity) {
    vector_t velo = body_get_velocity(shape);
    velo.y *= (-1 * elasticity);
    body_set_velocity(shape, velo);
}

// This function checks if the body has reached the floor, in which case, it
// should bounce!
void bounce_check(body_t *body, vector_t min) {
    list_t *shape = body_get_shape(body);
    for (size_t i = 0; i < list_size(shape); i++) {
        vector_t *current = list_get(shape, i);
        if (current->y <= min.y) {
            // Bounce and push the shape up so it is no longer beneath boundary.
            elastic_bounce(body, ELASTICITY);
            vector_t push = {
                .x = 0,
                .y = min.y - (polygon_centroid(shape).y - RADIUS)
            };
            polygon_translate(shape, push);
            break;
        }
    }
    list_free(shape);
}

// This function checks if a shape has gone past the right boundary. Returns
// true if it has.
int right_bound_check(body_t *body, vector_t max) {
    list_t *shape = body_get_shape(body);
    for (size_t i = 0; i < list_size(shape); i++) {
        vector_t *current = list_get(shape, i);
        if (current->x < max.x) {
            list_free(shape);
            return 1;
        }
    }
    list_free(shape);
    // If we reach here, there are no points to the left of the right boundary.
    return 0;
}

// This function returns a random float between 0 and 1.
float rand_float() {
    return (float) rand() / (float) RAND_MAX;
}

// This function "accelerates" the body by decreasing its y velocity over time
// like gravity.
void accelerate(body_t* body, double dt) {
    vector_t velo = body_get_velocity(body);
    velo.y -= ACCELERATION * dt;
    body_set_velocity(body, velo);
}

// This function initializes a star with prongs number of points and adds it
// to the list.
void import_star(scene_t * scene, int prongs) {
    float r = rand_float();
    float g = rand_float();
    float b = rand_float();
    body_t *star = create_star(prongs, r, g, b, FRAME_WIDTH);
    vector_t initial_velo = {
        .x = 5,
        .y = -3
    };
    body_set_velocity(star, initial_velo);
    scene_add_body(scene, star);
}


int main(int argc, char *argv[]) {
    // Stars are created at (0,0), make this the top left.
    vector_t max = {
        .x = FRAME_WIDTH * 2,
        .y = 0
    };
    vector_t min = {
        .x = 0,
        .y = -1 * FRAME_WIDTH
    };
    sdl_init(min, max);

    // Initialize number of prongs on stars
    int prong_count = 4;

    scene_t * scene = scene_init();
    import_star(scene, prong_count);
    prong_count += 1;

    // This value will represent the time since the last star was created.
    double star_time = 0;


    while (!sdl_is_done(scene)) {
        double dt = time_since_last_tick();

        sdl_clear();

        for (size_t i = 0; i < scene_bodies(scene); i++) {
            body_t * current_star = scene_get_body(scene, i);
            list_t * points = body_get_shape(current_star);
            vector_t velocity = body_get_velocity(current_star);
            rgb_color_t color = body_get_color(current_star);

            // Update coordinates
            body_set_centroid(current_star, vec_add(body_get_centroid(current_star),
                vec_multiply(dt, velocity)));

            // Update velocity
            accelerate(current_star, dt);
            bounce_check(current_star, min);

            // Rotate the star
            body_set_rotation(current_star, body_get_angle(current_star) +
                ROTATION_FACTOR);

            sdl_draw_polygon(points, color);

            list_free(points);

        }

        sdl_show();

        // Check if a star has moved past the boundary. If so, remove it and
        // free it.
        size_t j = 0;
        while (j < scene_bodies(scene)) {
            body_t * current_star = scene_get_body(scene, j);
            if (!right_bound_check(current_star, max)) {
                scene_remove_body(scene, j);
            }
            else {
                j++;
            }
        }

        // Check if it is time for a new star! If so, increment prong_count
        if (star_time > 1) {
            import_star(scene, prong_count);
            star_time = 0;
            prong_count += 1;
        }
        else {
            star_time += dt;
        }


    };

    scene_free(scene);
    return 0;
}
