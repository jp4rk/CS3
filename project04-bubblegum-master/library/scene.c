#include "scene.h"

const int STARTING_BODIES = 10;

struct scene {
    list_t *body_list;
    list_t *force_list;
};

struct force_package {
    force_creator_t forcer;
    void *aux;
    free_func_t freer;
};

void force_package_free(force_package_t *force) {
    if (force->aux != NULL) {
        force->freer(force->aux);
    }
    free(force);
}

scene_t *scene_init(void) {
    scene_t *scene = malloc(1 * sizeof(scene_t));
    assert(scene != NULL);
    scene->body_list = list_init(STARTING_BODIES, (free_func_t)body_free);
    scene->force_list = list_init(1, (free_func_t)force_package_free);
    return scene;
}

void scene_free(scene_t *scene) {
    list_free(scene->body_list);
    list_free(scene->force_list);
    free(scene);
}

size_t scene_bodies(scene_t *scene) {
    return list_size(scene->body_list);
}

size_t scene_forces(scene_t *scene) {
    return list_size(scene->force_list);
}

body_t *scene_get_body(scene_t *scene, size_t index) {
    assert(index >= 0 && index < scene_bodies(scene));
    return list_get(scene->body_list, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
    list_add(scene->body_list, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
    assert(index >= 0 && index < scene_bodies(scene));
    body_t *removed = list_remove(scene->body_list, index);
    body_free(removed);
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
    force_package_t force = {.forcer = forcer, .aux = aux, .freer = freer};
    force_package_t *force_ptr = malloc(1 * sizeof(force_package_t));
    assert(force_ptr != NULL);
    *force_ptr = force;
    list_add(scene->force_list, force_ptr);
}

void scene_tick(scene_t *scene, double dt) {
    for (size_t i = 0; i < list_size(scene->force_list); i++) {
        force_package_t *force = list_get(scene->force_list, i);
        force->forcer(force->aux);
    }
    for (size_t j = 0; j < list_size(scene->body_list); j++) {
        body_tick(scene_get_body(scene, j), dt);
    }
}
