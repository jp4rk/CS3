#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include "match.h"

void check(bool actual, char *pattern, char *text){
  char *cmd = malloc((strlen(pattern)+strlen(text)+50)*sizeof(char));
  strcpy(cmd, "echo \"");
  strcat(cmd, text);
  strcat(cmd, "\" | grep -c ^\"");
  strcat(cmd, pattern);
  strcat(cmd, "$\"");
  FILE *file = popen(cmd, "r");
  char *str = malloc(2*sizeof(char));
  str[0] = ' ';
  fread(str, 1, 1, file);
  if ((!actual && (str[0] == 1)) || (actual && (str[0] == 0))){
    printf("%c \n", str[0]);
  }
  pclose(file);
  free(cmd);
  free(str);
}

bool is_valid(char *pattern, int length){
  if(pattern[0] == '*')
    return false;
  for(size_t i = 0; i < length; ++i){
    char buffer = pattern[i];
    if(i != length -1 && buffer == '*' && pattern[i+1] == '*')
      return false;
    if(!isalpha(buffer) && buffer != '*' && buffer != '.')
      return false;
  }
  return true;
}

int LLVMFuzzerTestOneInput(uint8_t *data, size_t size) {
    size_t intsize = size / sizeof(int8_t);
    uint8_t *intdata = (uint8_t *)data;
    if (intsize >= 2) {
        size_t len1 = intdata[0];
        if (size >= sizeof(uint8_t) + len1) {
            size_t len2 = size - sizeof(uint8_t) - len1;
            char *str = ( (char *)intdata ) + 1;
            char *str1 = calloc(len1 + 1, sizeof(char));
            char *str2 = calloc(len2 + 1, sizeof(char));
            strncpy(str1, str, len1);
            strncpy(str2, str + len1, len2);
            if (is_valid(str1, len1) && is_valid(str2, len2))  {
                printf("s1=%s, s2=%s\n", str1, str2);
                bool result = match(str1, str2);
                check(result, str1, str2);
                free(str1);
                free(str2);
                return result;
            }
            free(str1);
            free(str2);
        }
    }
    return 0;
}
