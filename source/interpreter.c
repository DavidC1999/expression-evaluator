#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "interpreter.h"
#include "parser.h"


static float interpret_node(Node* node) {
    if(node == NULL) return 0.0;
    
    switch (node->kind)
    {
    case NODE_KIND_ADD: {
        Node* a = ((Node**)node->value)[0];
        Node* b = ((Node**)node->value)[1];
        float val_a = interpret_node(a);
        float val_b = interpret_node(b);
        return val_a + val_b;
    }
    case NODE_KIND_SUBTRACT: {
        Node* a = ((Node**)node->value)[0];
        Node* b = ((Node**)node->value)[1];
        float val_a = interpret_node(a);
        float val_b = interpret_node(b);
        return val_a - val_b;
    }
    case NODE_KIND_MULTIPLY: {
        Node* a = ((Node**)node->value)[0];
        Node* b = ((Node**)node->value)[1];
        float val_a = interpret_node(a);
        float val_b = interpret_node(b);
        return val_a * val_b;
    }
    case NODE_KIND_DIVIDE: {
        Node* a = ((Node**)node->value)[0];
        Node* b = ((Node**)node->value)[1];
        float val_a = interpret_node(a);
        float val_b = interpret_node(b);
        return val_a / val_b;
    }
    case NODE_KIND_POW: {
        Node* a = ((Node**)node->value)[0];
        Node* b = ((Node**)node->value)[1];
        float val_a = interpret_node(a);
        float val_b = interpret_node(b);
        return pow(val_a, val_b);
    }
    case NODE_KIND_MINUS: {
        Node* a = (Node*)node->value;
        float val_a = interpret_node(a);
        return val_a * -1.0;
    }
    case NODE_KIND_NUMBER: {
        float num = *(float*)node->value;
        return num;
    }
    
    default:
        fprintf(stderr, "Unhandled node in interpret_node()\n");
        exit(1);
    }

}

Interpreter* create_interpreter(Parser* parser) {
    Interpreter* interpreter = malloc(sizeof(Interpreter));
    *interpreter = (Interpreter){
        .parser = parser
    };
    return interpreter;
}

float interpret(Interpreter* interpreter) {
    Parser* parser = interpreter->parser;
    return interpret_node(parser->root);
}

void free_interpreter(Interpreter* interpreter) {
    free_parser(interpreter->parser);
    free(interpreter);
}
