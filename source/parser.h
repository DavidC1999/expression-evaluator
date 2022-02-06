#ifndef _PARSER_H
#define _PARSER_H

#include "tokenizer.h"

typedef enum {
    NODE_KIND_NUMBER,   // 0
    NODE_KIND_ADD,      // 1
    NODE_KIND_SUBTRACT, // 2
    NODE_KIND_MULTIPLY, // 3
    NODE_KIND_DIVIDE,   // 4
    NODE_KIND_POW,      // 5
    NODE_KIND_MINUS,    // 6
} NodeKind;

typedef struct {
    NodeKind kind;
    void* value;
} Node;

typedef struct {
    Node* root;
    Tokenizer* tokenizer;
} Parser;

Parser* create_parser(Tokenizer* tokenizer);
void parse(Parser* parser);
void free_parser(Parser* parser);
void print_tree(Node* node);

#endif // _PARSER_H