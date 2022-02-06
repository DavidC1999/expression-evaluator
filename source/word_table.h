#ifndef _WORD_TABLE_H
#define _WORD_TABLE_H

#include <stdbool.h>
#include <string.h>

#include "parser.h"

// #define NEW_WORD(new_word) strncmp(word, new_word, strlen(new_word))

bool word_is_function(char* word) {
    return strncmp(word, "sqrt", 4) == 0 ||
           strncmp(word, "ln", 2) == 0 ||
           strncmp(word, "log", 3) == 0 ||
           strncmp(word, "pi", 2) == 0 ||
           strncmp(word, "e", 1) == 0;
}

#endif // _WORD_TABLE_H
