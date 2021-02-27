#include "vec_list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

 /*
 Constructs a growable array of vectors, each of which are stored as pointers to
 malloc()ed vectors.
 */
typedef struct vec_list {
  size_t size;
  void **list;
  int capacity;
} vec_list_t;

// Returns a list with the memory allocated for a given initial size
vec_list_t *vec_list_init(size_t initial_size){
  vec_list_t *ans = malloc(sizeof(vec_list_t));
  assert(ans != NULL);
  ans -> list = malloc(sizeof(void *) * initial_size);
  assert(ans -> list != NULL);
  ans -> size = 0;
  ans -> capacity = initial_size;
  return ans;
}

// Frees all vectors in the list
void vec_list_free(vec_list_t *list){
  for(size_t i = 0; i < list->size; i++){
    free(list->list[i]);
  }
  free(list-> list);
  free(list);
}

//Returns the number of occupied elements of the list.
size_t vec_list_size(vec_list_t *list){
  return list->size;
}

// Returns the element at a given index in a list.
void *vec_list_get(vec_list_t *list, size_t index){
  assert(index >= 0 && index < list->size);
  return list->list[index];
}

// Returns a new vec_list with twice the occupied size. Elements are copied
void vec_list_resize(vec_list_t *list){
  list->list = realloc(list->list, 2 * sizeof(void *) * list->capacity);
  list->capacity = list->capacity * 2;
}

// Adds a value at the end of the list.
void vec_list_add(vec_list_t *list, void *value){
  assert(value != NULL && list->size < list->capacity);
  list->list[list->size] = value;
  list->size ++;
  // if(list->size == list->capacity){
  //   vec_list_resize(list);
  // }
}

// Adds a value at the front of the list.
void vec_list_add_front(vec_list_t *list, void *value){
  list->size ++;
  // if(list->size == list->capacity){
  //   vec_list_resize(list);
  // }
  for(size_t i = vec_list_size(list)-1; i > 0; i--) {
    void *temp = list->list[i-1];
    list->list[i] = temp;
  }
  list->list[0] = value;
}

// Removes the element at the end of a list. Returns the removed element.
void *vec_list_remove(vec_list_t *list){
  assert(list->size > 0);
  void *ans = list->list[list->size-1];
  list->size --;
  return ans;
}

// Removes an element at a specified index. Returns the removed element.
void *vec_list_remove_index(vec_list_t *list, size_t index){
  assert(list->size > 0);
  void *ans = list->list[index];
  for(size_t i = index; i < vec_list_size(list) -1; i++) {
    list->list[i] = list->list[i+1];
    }
  list->size--;
  return ans;
}
