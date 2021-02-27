#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

typedef struct vector {
    double x;
    double y;
} vector_t;


vector_t *vec_init(double x, double y){
    vector_t *ans =  malloc(sizeof(vector_t));
    ans -> x = x;
    ans -> y = y;
    return ans;
}

void vec_free(vector_t *vec){
    free(vec);
}

vector_t *vec_add(vector_t *v1, vector_t *v2){
    return vec_init(v1->x + v2->x, v1->y + v2->y);
}


int main(int argc, char* argv[]) {
    vector_t *var1 = vec_init(10, 10);
    vector_t *var2 = vec_init(20, 20);
    vector_t *var3 = vec_init(30, 30);
    vector_t *var4 = vec_add(var2, var3);
    printf("%f\n", var2 -> x);
    printf("%f\n", var2 -> y);
    printf("%f\n", var4 -> x);
    printf("%f\n", var4 -> y);
    vec_free(var1);
    vec_free(var2);
    vec_free(var3);
    vec_free(var4);
}
