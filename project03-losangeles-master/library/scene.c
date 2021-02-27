#include "scene.h"


struct scene {
    list_t *body_list;
};

scene_t *scene_init(void) {
    scene_t *scene = malloc(1 * sizeof(scene_t));
    assert(scene != NULL);
    scene->body_list = list_init(10, (free_func_t) body_free);
    return scene;
}

void scene_free(scene_t *scene) {
    list_free(scene->body_list);
    free(scene);
}

size_t scene_bodies(scene_t *scene) {
    return list_size(scene->body_list);
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

void scene_tick(scene_t *scene, double dt) {
    for (size_t i = 0; i < list_size(scene->body_list); i++) {
        body_tick(scene_get_body(scene, i), dt);
    }
}
