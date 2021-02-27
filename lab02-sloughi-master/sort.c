#include "sort.h"

void ascending_int_sort_whole(int arr[], size_t nelements) {
  ascending_int_sort(arr, 0, nelements);
}

void ascending_int_sort(int arr[], size_t lo, size_t hi) {
  int_sort(arr, lo, hi, (int_comparator_t) (int_asc));
}

void descending_int_sort(int arr[], size_t lo, size_t hi) {
  int_sort(arr, lo, hi, (int_comparator_t) (int_desc));
}

void int_sort(int arr[], size_t lo, size_t hi, int_comparator_t compare) {
  for(int i = lo + 1; i < hi; ++i){
    int pivot = arr[i];
    int ind = i;
    while(ind > lo && compare(arr[ind], arr[ind-1]) < 0){
      int temp = arr[ind-1];
      arr[ind-1] = arr[ind];
      arr[ind] = temp;
      ind--;
    }
  }
}

void string_sort(char *arr[], size_t lo, size_t hi, string_comparator_t compare) {
  sort((void **) arr, lo, hi, (comparator_t) compare);
}

void sort(void *arr[], size_t lo, size_t hi, comparator_t compare) {
  for(int i = lo + 1; i < hi; ++i){
    void *pivot = arr[i];
    int ind = i;
    while(ind > lo && compare(arr[ind], arr[ind-1]) < 0){
      void *temp = arr[ind-1];
      arr[ind-1] = arr[ind];
      arr[ind] = temp;
      ind--;
    }
  }
}
