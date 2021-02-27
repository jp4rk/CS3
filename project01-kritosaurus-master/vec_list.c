#include "vec_list.h"
#include <assert.h>

typedef struct vec_list{
  size_t size;
  vector_t **list;
} vec_list_t;

vec_list_t *vec_list_init(size_t initial_size){
  vec_list_t *ans = malloc(initial_size * sizeof(vector_t));
  for(int i = 0; i < initial_size; ++i)
    ans[i] = malloc(sizeof(vector_t));
  assert(ans != NULL);
  ans -> list = malloc(sizeof(vector_t) * initial_size);
  assert(ans -> list != NULL);
  ans -> size = initial_size;
  return ans;
}

void vec_list_free(vec_list_t *list){
  for(int i = 0; i < list -> size; ++i){
    free(list[i]);
  }
  free(list);
}

size_t vec_list_size(vec_list_t *list){
  return list -> size;
}


vector_t *vec_list_get(vec_list_t *list, size_t index){
  assert(index >= 0 && index < list -> size);
  return list[index];
}


void vec_list_add(vec_list_t *list, vector_t *value){
  assert(sizeof(list) >  vec_list_size(list) && value != NULL);
  list[list->size] = value;
  list->size += 1;
}

vector_t *vec_list_remove(vec_list_t *list){
  assert(list -> size != 0);
  int ind = list -> size - 1;
  vector_t ans = list[ind];
  list -> size -= 1;
  list[ind] = NULL;
  return ans;
}
