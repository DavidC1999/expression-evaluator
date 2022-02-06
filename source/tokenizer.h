#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <stdint.h>

typedef enum {
    TOKEN_KIND_LPAREN,
    TOKEN_KIND_RPAREN,
    TOKEN_KIND_NUMBER,
    TOKEN_KIND_WORD,
    TOKEN_KIND_MINUS,
    TOKEN_KIND_PLUS,
    TOKEN_KIND_MULTIPLY,
    TOKEN_KIND_DIVIDE,
    TOKEN_KIND_CARET,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    void* value;
    Token* next;

    uint32_t column;
};

typedef struct {
    Token* head;
    Token* last;
    Token* current; // used for iteration
    uint32_t _curr_col; // used internally by tokenizer
} Tokenizer;


Tokenizer* create_tokenizer();
void free_tokenizer(Tokenizer* tokenizer);
void tokenize_str(Tokenizer* tokenizer, char* str);
void print_tokens(Token* token);
Token* tokenizer_curr(Tokenizer* tokenizer);
Token* tokenizer_next(Tokenizer* tokenizer);

#endif // _TOKENIZER_H_