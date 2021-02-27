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

    char *temp = malloc(1 * sizeof(char));
    char *ans = malloc(101*sizeof(char));
    char *word = malloc(101*sizeof(char));

    if (temp == NULL || word == NULL || ans == NULL){
        return 1;
    }

    word[0] = '\0';
    ans[0] = '\0';

    int word_index = 0;

    while (1) {
        fread(temp, 1, 1, fp);
        if (feof(fp)) {
            break;
        }

        if (temp[0] == ' ' || temp[0] == '\n'){
            if (strlen(word) > strlen(ans)){
                ans = strcpy(ans, word);
            }
            word[0] = '\0';
	          word_index = 0;
        }
        else {
            word[word_index] = temp[0];
            word_index += 1;
            word[word_index] = '\0';
        }
    }

    if (strlen(ans) != 0){
        printf("%s\n", ans);
    }

    free(temp);
    free(ans);
    free(word);
    fclose(fp);
    return 0;
}
