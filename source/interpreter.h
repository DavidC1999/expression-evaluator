#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "parser.h"

typedef struct {
    Parser* parser;
} Interpreter;

Interpreter* create_interpreter(Parser* parser);
float interpret(Interpreter* interpreter);
void free_interpreter(Interpreter* interpreter);

#endif // _INTERPRETER_H