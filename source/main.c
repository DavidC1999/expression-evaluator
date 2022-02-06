#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"

#define UI_SIZE 100

float calculate(bool debug_info, char* str) {
    if(debug_info) {
        printf("----------------\n");
        printf("User input: \n");
        printf("----------------\n");
        printf("%s\n\n", str);
    }

    Tokenizer* tokenizer = create_tokenizer();
    tokenize_str(tokenizer, str);

    if(debug_info) {
        printf("----------------\n");
        printf("Tokens: \n");
        printf("----------------\n");
        print_tokens(tokenizer->head);
        printf("\n");
    }

    Parser* parser = create_parser(tokenizer);
    parse(parser);

    if(debug_info) {
        printf("----------------\n");
        printf("Parsed tree: \n");
        printf("----------------\n");
        print_tree(parser->root);
        printf("\n\n");
    }
    Interpreter* interpreter = create_interpreter(parser);
    float result = interpret(interpreter);

    if(debug_info) {
        printf("----------------\n");
        printf("Result: \n");
        printf("----------------\n");
        printf("%f\n\n", result);
    }
    free_interpreter(interpreter);

    return result;
}

void remove_newline(char* str) {
    size_t input_len = strlen(str);

    for(size_t i = 0; i < input_len; ++i) {
        if(str[i] == '\n' || str[i] == '\r') {
            str[i] = '\0';
            break;
        }
    }
}

int main(int argc, char** argv) {
    --argc; ++argv; // consume program name

    bool debug = false;
    if(argc > 0 && strcmp(*argv, "-d") == 0) { // debug
        debug = true;
        printf("Printing debug\n");
    }

    printf("\"exit\" to quit\n");

    char user_input[UI_SIZE] = {0};
    while(true) {
        printf(" > ");
        fgets(user_input, UI_SIZE, stdin);
        remove_newline(user_input);

        if(strcmp(user_input, "exit") == 0) break;
        float result = calculate(debug, user_input);
        printf("%f\n", result);
    }

    return 0;
}