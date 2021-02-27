#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int assert(FILE* fp, char* file){
    if (fp == NULL){
        printf("wc: %s: No such file or directory\n", file);
        return 0;
    }
    return 1;
}

int check_usage(int args, int comp, char* filename){
    if (args != comp){
        printf("Usage: %s <filename>\n", filename);
        return 0;
    }
    return 1;
}

char* allocate_memory(int num){
    return malloc(num * sizeof(char));
}

int main(int argc, char *argv[]) {

    if (!check_usage(argc, 2, argv[0])) {
      printf("Usage: %s <filename>\n", argv[0]);
      return 1;
    }

    char* filename = argv[1];
    FILE *fp = fopen(filename, "r");

    if(!assert(fp, filename)) {
      return 1;
    }

    int num_lines = 0;
    char *result = allocate_memory(1);

    if (result == NULL){
        return 1;
    }

    while (1) {
        fread(result, 1, 1, fp);
        if (feof(fp)) {
            break;
        }
        if (result[0] == '\n'){
            num_lines++;
    	  }
    }

    printf("%d %s", num_lines, filename);
    printf("\n");
    free(result);
    fclose(fp);
    return 0;
}
