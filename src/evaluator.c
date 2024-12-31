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

object_t eval(void *node, node_type_t node_type){
    switch(node_type){
        case NODE_PROGRAM:{
            program_t *program = (program_t *)node;
            return eval_program_node(program);
        }
        case NODE_EXPRESSION:{
            expression_t *expr = (expression_t *)node;
            return eval_expression_node(expr);
        }
        case NODE_STATEMENT:{
            statement_t *sttmnt = (statement_t *)node;
            return eval_expression_node(sttmnt->value);
        }
        default:
            return (object_t){};
    }
}

object_t eval_program_node(program_t *program){
    object_t result;
    for(int i = 0; i < program->statements->count; i++){
         result = eval(program->statements->data[i], NODE_STATEMENT);
    }
    return result;
}

object_t eval_expression_node(expression_t *expression){
    switch(expression->type){
        case INTEGER_LITERAL:
            return (object_t){
                .type = OBJECT_INTEGER,
                .integer = expression->integer
            };
        case BOOLEAN_EXPR:
            return (object_t){
                .type = OBJECT_BOOLEAN,
                .boolean = expression->boolean
            };
        default:
            return (object_t){};
    }
}

void inspect_object(object_t object, char *buff_out){
    switch(object.type){
        case OBJECT_INTEGER:
            snprintf(buff_out, BUFSIZ, "%d\n", object.integer);
            break;
        case OBJECT_BOOLEAN:
            snprintf(buff_out, BUFSIZ, "%s\n", object.boolean ? "true" : "false");
            break;
        default:
            snprintf(buff_out, BUFSIZ, "Unknown object type\n");
            break;
    }
}

