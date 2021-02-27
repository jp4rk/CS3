#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "forces.h"
#include "test_util.h"

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

void test_drag() {
    const double GAMMA = 0.05;
    const double M = 2;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass = body_init(make_shape(), M, (rgb_color_t){0, 0, 0});
    vector_t velo = (vector_t){1, 1};
    body_set_velocity(mass, velo);
    scene_add_body(scene, mass);
    create_drag(scene, GAMMA, mass);
    for (int i = 0; i < STEPS; i++) {
        velo = vec_add(velo, vec_multiply(DT * GAMMA / M, vec_negate(velo)));
        scene_tick(scene, DT);
        assert(vec_isclose(body_get_velocity(mass), velo));
    }
    scene_free(scene);
}

double spring_potential(double K, body_t *body1, body_t *body2) {
    vector_t difference =
        vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    double distance = sqrt(difference.x * difference.x + difference.y * difference.y);
    return .5 * K * distance * distance;
}

double kinetic_energy(body_t *body) {
    vector_t v = body_get_velocity(body);
    return body_get_mass(body) * vec_dot(v, v) / 2;
}

void test_energy_conservation_spring() {
    const double M1 = 4.5, M2 = 7.3;
    const double K = 2;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass1 = body_init(make_shape(), M1, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, mass1);
    body_t *mass2 = body_init(make_shape(), M2, (rgb_color_t){0, 0, 0});
    body_set_centroid(mass2, (vector_t){10, 20});
    scene_add_body(scene, mass2);
    double initial_energy = spring_potential(K, mass1, mass2);
    for (int i = 0; i < STEPS; i++) {
        assert(body_get_centroid(mass1).x < body_get_centroid(mass2).x);
        double energy = spring_potential(K, mass1, mass2) + kinetic_energy(mass1) +
                        kinetic_energy(mass2);
        assert(within(1e-4, energy / initial_energy, 1));
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

// Test that forces are added properly
void test_gravity_with_drag() {
    const double M1 = 10;
    const double M2 = 20;
    const double GAMMA = 0.05;
    const double A = 10;
    const double G = 15;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *body1= body_init(make_shape(), M1, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, body1);
    body_t *body2 = body_init(make_shape(), M2, (rgb_color_t){0, 0, 0});
    body_set_centroid(body2, (vector_t){0, A});
    scene_add_body(scene, body2);
    create_drag(scene, GAMMA, body1);
    create_newtonian_gravity(scene, G, body1, body2);
    vector_t velo = VEC_ZERO;
    for (int i = 0; i < STEPS; i++) {
        vector_t force = vec_multiply(GAMMA, vec_negate(velo));
        vector_t difference =
            vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
        double distance = sqrt(difference.x * difference.x + difference.y * difference.y);
        double coef = G * M1 * M2 / (distance * distance);
        vector_t unit_vec = vec_multiply(1 / distance, difference);
        vector_t result = vec_multiply(coef, unit_vec);
        force = vec_add(force, result);
        velo = vec_add(velo, vec_multiply(DT / M1, force));
        scene_tick(scene, DT);
        assert(vec_isclose(body_get_velocity(body1), velo));
    }
    scene_free(scene);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_drag);
    DO_TEST(test_energy_conservation_spring);
    DO_TEST(test_gravity_with_drag);

    puts("forces_test PASS");
}
