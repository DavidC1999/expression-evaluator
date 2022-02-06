#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "word_table.h"

static void free_parser_tree(Node* node) {
    if (node == NULL) {
        return;
    }

    switch (node->kind) {
        case NODE_KIND_ADD:
        case NODE_KIND_SUBTRACT:
        case NODE_KIND_MULTIPLY:
        case NODE_KIND_DIVIDE:
        case NODE_KIND_POW:
            free_parser_tree(((Node**)node->value)[0]);
            free_parser_tree(((Node**)node->value)[1]);
            break;
        case NODE_KIND_NUMBER:
            free(node->value);
            break;
        case NODE_KIND_MINUS:
            free_parser_tree((Node*)node->value);
            break;
    }
    free(node);
}

void free_parser(Parser* parser) {
    free_parser_tree(parser->root);
    free_tokenizer(parser->tokenizer);
    free(parser);
}

void print_tree(Node* node) {
    switch (node->kind) {
        case NODE_KIND_ADD: {
            Node** values = (Node**)node->value;
            printf("(");
            print_tree(values[0]);
            printf("+");
            print_tree(values[1]);
            printf(")");
            break;
        }
        case NODE_KIND_SUBTRACT: {
            Node** values = (Node**)node->value;
            printf("(");
            print_tree(values[0]);
            printf("-");
            print_tree(values[1]);
            printf(")");
            break;
        }
        case NODE_KIND_MULTIPLY: {
            Node** values = (Node**)node->value;
            printf("(");
            print_tree(values[0]);
            printf("*");
            print_tree(values[1]);
            printf(")");
            break;
        }
        case NODE_KIND_DIVIDE: {
            Node** values = (Node**)node->value;
            printf("(");
            print_tree(values[0]);
            printf("/");
            print_tree(values[1]);
            printf(")");
            break;
        }
        case NODE_KIND_POW: {
            Node** values = (Node**)node->value;
            printf("(");
            print_tree(values[0]);
            printf("^");
            print_tree(values[1]);
            printf(")");
            break;
        }
        case NODE_KIND_NUMBER: {
            float* value = (float*)node->value;
            printf("%f", *value);
            break;
        }
        case NODE_KIND_MINUS: {
            Node* value = (Node*)node->value;
            printf("(-");
            print_tree(value);
            printf(")");
            break;
        }
    }
}

static void panic(Parser* parser, char* reason) {
    Token* token = tokenizer_curr(parser->tokenizer);
    if (tokenizer_curr(parser->tokenizer) == NULL) {
        token = parser->tokenizer->last;
    }
    fprintf(stderr, "Paniced in parsing at column %d: %s\n", token->column, reason);
    free_parser(parser);
    exit(1);
}

static Node* get_expr(Parser* parser);
static Node* get_factor(Parser* parser) {
    Tokenizer* tokenizer = parser->tokenizer;
    Token* token = tokenizer_next(tokenizer);

    if (token == NULL) {
        panic(parser, "unexpected end");
    }

    if (token->kind == TOKEN_KIND_NUMBER) {
        Node* output = malloc(sizeof(Node));

        float* value = malloc(sizeof(float));
        *value = *(float*)token->value;

        *output = (Node){
            .kind = NODE_KIND_NUMBER,
            .value = value};

        return output;
    } else if (token->kind == TOKEN_KIND_LPAREN) {
        Node* output = malloc(sizeof(Node));
        output = get_expr(parser);
        Token* node_rparen = tokenizer_curr(tokenizer);
        if (node_rparen == NULL) panic(parser, "unexpected end");
        if (node_rparen->kind != TOKEN_KIND_RPAREN) panic(parser, "unexpected token");
        tokenizer_next(tokenizer);  // consume ')'
        return output;
    } else if (token->kind == TOKEN_KIND_MINUS) {
        Node* output = malloc(sizeof(Node));
        *output = (Node){
            .kind = NODE_KIND_MINUS,
            .value = get_factor(parser)};
        return output;
    }

    panic(parser, "unexpected token");
    return NULL;
}

static Node* get_power(Parser* parser) {
    Tokenizer* tokenizer = parser->tokenizer;

    Node* result = get_factor(parser);

    if (tokenizer_curr(tokenizer) != NULL &&
           (tokenizer_curr(tokenizer)->kind == TOKEN_KIND_CARET)) {
        tokenizer_next(tokenizer);  // consume ^

        Node** value = malloc(sizeof(Node*) * 2);
        value[0] = result;
        value[1] = get_power(parser);

        result = malloc(sizeof(Node));
        *result = (Node){
            .kind = NODE_KIND_POW,
            .value = value};
    }
    return result;
}

static Node* get_term(Parser* parser) {
    Tokenizer* tokenizer = parser->tokenizer;

    // Node* result = get_factor(parser);
    Node* result = get_power(parser);

    while (tokenizer_curr(tokenizer) != NULL &&
           (tokenizer_curr(tokenizer)->kind == TOKEN_KIND_MULTIPLY || tokenizer_curr(tokenizer)->kind == TOKEN_KIND_DIVIDE)) {
        TokenKind token_kind = tokenizer_next(tokenizer)->kind;  // also consumes * or /

        NodeKind node_kind = token_kind == TOKEN_KIND_MULTIPLY ? NODE_KIND_MULTIPLY : NODE_KIND_DIVIDE;

        // Node* node = malloc(sizeof(Node));
        Node** value = malloc(sizeof(Node*) * 2);
        value[0] = result;
        value[1] = get_factor(parser);

        result = malloc(sizeof(Node));
        *result = (Node){
            .kind = node_kind,
            .value = value};
    }
    return result;
}

static Node* get_expr(Parser* parser) {
    Tokenizer* tokenizer = parser->tokenizer;

    Node* result = get_term(parser);

    while (tokenizer_curr(tokenizer) != NULL && (tokenizer_curr(tokenizer)->kind == TOKEN_KIND_PLUS || tokenizer_curr(tokenizer)->kind == TOKEN_KIND_MINUS)) {
        TokenKind token_kind = tokenizer_next(tokenizer)->kind;  // also consumes + or -

        NodeKind node_kind = token_kind == TOKEN_KIND_PLUS ? NODE_KIND_ADD : NODE_KIND_SUBTRACT;

        // Node* node = malloc(sizeof(Node));
        Node** value = malloc(sizeof(Node*) * 2);
        value[0] = result;
        value[1] = get_term(parser);

        result = malloc(sizeof(Node));
        *result = (Node){
            .kind = node_kind,
            .value = value};
    }
    return result;
}

void parse(Parser* parser) {
    Tokenizer* tokenizer = parser->tokenizer;
    if (tokenizer_curr(tokenizer) == NULL) {
        return;
    }

    parser->root = get_expr(parser);

    if (tokenizer_curr(tokenizer) != NULL) {
        panic(parser, "Invalid syntax");
    }
}

Parser* create_parser(Tokenizer* tokenizer) {
    Parser* parser = malloc(sizeof(Parser));

    *parser = (Parser){
        .root = NULL,
        .tokenizer = tokenizer};
    return parser;
}