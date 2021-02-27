#include "match.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

bool match_char(char one, char two){
  if(one == two)
    return true;
  else if (one == '.' && two != '\0')
    return true;
  else
    return false;
}

// [.]*b*c* | a  aaaaaaaabbbbccccccc

// Recursively tests characters to determine if strings match
bool help_match(char *pattern, char *text){
  if(strlen(pattern) == 0 && strlen(text) == 0)
      return true;
  else if (strlen(pattern) == 0 && strlen(text) != 0)
      return false;
  else if (pattern[1] == '*'){
    if(match_char(pattern[0], text[0])){
      if(text[0] != '\0')
          return help_match(pattern, text+1) || help_match(pattern+2, text);
      return help_match(pattern+2, text);
    }
    else
      return help_match(pattern+2, text);
  }

  else if (match_char(pattern[0], text[0])){
    return help_match(pattern + 1, text + 1);
  }
  else
    return false;
}
// Function to run the matches
bool match(char *pattern, char *text) {
  return help_match(pattern, text);
}
