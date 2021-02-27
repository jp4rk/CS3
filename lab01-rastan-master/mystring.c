#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mystring.h"

strarray *strsplit(const char *str) {
  strarray *ans = malloc(2 * sizeof(strarray));
  char **words = malloc((strlen(str)+1) * sizeof(char *));
  for(int i = 0; i < strlen(str)+1; ++i)
    words[i] = malloc((strlen(str)+1) * sizeof(char));

  char *temp = malloc((strlen(str) + 1) * sizeof(char));
  for(int i = 0; i < strlen(str); ++i)
    temp[i] = 0;

  int counter = 0;
  int length = 0;

// The bus is here
  for(int i = 0; i < strlen(str); ++i){
    if(str[i] == ' '){
  		if(strlen(temp) != 0){
  			temp[counter] = '\0';
  			strcpy(words[length], temp);
  			length++;
        temp[0] = '\0';
  		}
	    counter = 0;
	  }
    else {
      temp[counter] = str[i];
      counter++;
    }
  }

  if(strlen(str) == 1){
    temp[0] = str[0];
    temp[1] = '\0';
  }

  if(strlen(temp) != 0){
    temp[counter] = '\0';
    strcpy(words[length], temp);
    length++;
  }

  ans->data = words;
  ans->length = length;
  free(temp);
  return ans;
}
