#include "tokenizer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void panic(Tokenizer* tokenizer, char* reason) {
    fprintf(stderr, "Paniced in tokenization at column %d: %s\n", tokenizer->_curr_col, reason);
    free_tokenizer(tokenizer);
    exit(1);
}

static void free_token(Token* token) {
    free(token->value);
    free(token);
}

static void free_tokens(Token* token) {
    if (token == NULL) return;
    free_tokens(token->next);
    free_token(token);
}

void free_tokenizer(Tokenizer* tokenizer) {
    free_tokens(tokenizer->head);
    free(tokenizer);
}

Tokenizer* create_tokenizer() {
    Tokenizer* tokenizer = malloc(sizeof(Tokenizer));

    *tokenizer = (Tokenizer){
        .head = NULL,
        .last = NULL,
        .current = NULL,
        ._curr_col = 0};

    return tokenizer;
}

Token* tokenizer_curr(Tokenizer* tokenizer) {
    return tokenizer->current;
}

Token* tokenizer_next(Tokenizer* tokenizer) {
    Token* temp = tokenizer_curr(tokenizer);
    if(temp == NULL) return NULL;
    tokenizer->current = tokenizer->current->next;
    return temp;
}

///////////////// CONSTRUCT TOKEN FUNCTIONS

static void append_token(Tokenizer* tokenizer, Token* token) {
    if (tokenizer->head == NULL) {
        tokenizer->head = token;
        tokenizer->last = token;
        return;
    }

    assert(tokenizer->last->next == NULL);
    assert(token->next == NULL);
    tokenizer->last->next = token;
    tokenizer->last = token;
}

static bool is_digit(char c);  // construct_number_token() depends on is_digit()
static uint32_t construct_number_token(Tokenizer* tokenizer, char* str, uint32_t index) {
    char accu[50];
    uint8_t accu_index = 0;

    uint8_t dot_amt = 0;
    while (is_digit(str[index])) {
        if (str[index] == '.') ++dot_amt;
        accu[accu_index++] = str[index++];
    }

    if (dot_amt > 1) {
        char buffer[100];
        snprintf(buffer, 100, "Unknown character for constructing number token: %c", str[index]);
        panic(tokenizer, buffer);
    }

    accu[accu_index] = '\0';
    float* num = malloc(sizeof(float));
    *num = atof(accu);

    Token* token = malloc(sizeof(Token));
    *token = (Token){
        .kind = TOKEN_KIND_NUMBER,
        .value = num,
        .next = NULL,
        .column = tokenizer->_curr_col};
    append_token(tokenizer, token);

    return accu_index;
}

static uint32_t construct_word_token(Tokenizer* tokenizer, char* str) {
    size_t len = strlen(str);
    char* word = malloc(sizeof(char) * (len + 1));
    strcpy(word, str);

    Token* token = malloc(sizeof(Token));
    *token = (Token){
        .kind = TOKEN_KIND_WORD,
        .value = word,
        .next = NULL,
        .column = tokenizer->_curr_col};
    append_token(tokenizer, token);

    return len;
}

static void construct_op_token(Tokenizer* tokenizer, char c) {
    TokenKind kind;
    switch (c) {
        case '+':
            kind = TOKEN_KIND_PLUS;
            break;
        case '-':
            kind = TOKEN_KIND_MINUS;
            break;
        case '*':
            kind = TOKEN_KIND_MULTIPLY;
            break;
        case '/':
            kind = TOKEN_KIND_DIVIDE;
            break;
        case '^':
            kind = TOKEN_KIND_CARET;
            break;
        default: {
            char buffer[50];
            snprintf(buffer, 50, "invalid operation character: %c", c);
            panic(tokenizer, buffer);
        }
    }

    Token* token = malloc(sizeof(Token));
    *token = (Token){
        .kind = kind,
        .value = NULL,
        .next = NULL,
        .column = tokenizer->_curr_col};
    append_token(tokenizer, token);
}

static void construct_single_char_token(Tokenizer* tokenizer, TokenKind kind) {
    Token* token = malloc(sizeof(Token));
    *token = (Token){
        .kind = kind,
        .value = NULL,
        .next = NULL,
        .column = tokenizer->_curr_col};
    append_token(tokenizer, token);
}

///////////////// SIMPLE HELPER FUNCTIONS

static bool is_word_char(char c) {
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '_';
}

static bool is_special_op(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
            return true;
        default:
            return false;
    }
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9') || c == '.';
}

static bool should_ignore(char c) {
    switch (c) {
        case '\r':
        case ' ':
            return true;
    }
    return false;
}

static void get_word_str(Tokenizer* tokenizer, char* buffer, uint32_t buffer_size, char* str, uint32_t index) {
    uint32_t buffer_index = 0;
    while (is_word_char(str[index])) {
        if (buffer_index + 1 >= buffer_size) {
            panic(tokenizer, "word too long");
        }
        buffer[buffer_index++] = str[index++];
    }
    if (buffer_index >= buffer_size) {
        panic(tokenizer, "word too long");
    }
    buffer[buffer_index] = '\0';
}

void print_token(Token* token) {
    switch (token->kind) {
        case TOKEN_KIND_NUMBER:
            printf("<number: %f>\n", *(float*)token->value);
            break;
        case TOKEN_KIND_LPAREN:
            printf("<lparen>\n");
            break;
        case TOKEN_KIND_RPAREN:
            printf("<rparen>\n");
            break;
        case TOKEN_KIND_WORD:
            printf("<word: \"%s\">\n", (char*)token->value);
            break;
        case TOKEN_KIND_MINUS:
            printf("<minus>\n");
            break;
        case TOKEN_KIND_PLUS:
            printf("<plus>\n");
            break;
        case TOKEN_KIND_MULTIPLY:
            printf("<multiply>\n");
            break;
        case TOKEN_KIND_DIVIDE:
            printf("<divide>\n");
            break;
        case TOKEN_KIND_CARET:
            printf("<caret>\n");
            break;
        default:
            printf("token printing of this type is not implemented: %d\n", token->kind);
    }
}

void print_tokens(Token* token) {
    if (token == NULL) {
        printf("end\n");
        return;
    }

    print_token(token);

    print_tokens(token->next);
}

void tokenize_str(Tokenizer* tokenizer, char* str) {
    uint32_t index = 0;

    while (str[index] != '\0') {
        if (should_ignore(str[index])) {
            ++index;                        // consume character to be ignored
        } else if (is_digit(str[index])) {  // numbers
            uint32_t advanced = construct_number_token(tokenizer, str, index);
            index += advanced;
        } else if (str[index] == '(') {
            construct_single_char_token(tokenizer, TOKEN_KIND_LPAREN);
            ++index;
        } else if (str[index] == ')') {
            construct_single_char_token(tokenizer, TOKEN_KIND_RPAREN);
            ++index;
        } else if (is_special_op(str[index])) {  // basic math operation like '*' or '+'
            construct_op_token(tokenizer, str[index]);
            ++index;
        } else if (is_word_char(str[index])) {
            char buffer[50];
            get_word_str(tokenizer, buffer, 50, str, index);
            uint32_t advanced = construct_word_token(tokenizer, buffer);
            index += advanced;
        } else {
            char buffer[30];
            snprintf(buffer, 30, "Unknown character: '%c'", str[index]);
            panic(tokenizer, buffer);
        }

        ++(tokenizer->_curr_col);
    }

    tokenizer->current = tokenizer->head;
}
