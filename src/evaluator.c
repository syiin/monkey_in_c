#include <stdio.h>
#include "evaluator.h"
#include "ast.h"

char *object_type_to_string(object_type_t object_type){
    switch(object_type){
        case OBJECT_INTEGER:
            return "OBJECT_INTEGER";
        case OBJECT_BOOLEAN:
            return "OBJECT_BOOLEAN";
        default:
            return "";
    }
}

object_t eval(expression_t *expression){
    switch(expression->type){
        case INTEGER_LITERAL:
            return (object_t){
                .type = OBJECT_INTEGER,
                .integer = expression->integer
            };
        default:
            return (object_t){};
    }
}
