#include <stdio.h>
#include <string.h>
#include "evaluator.h"
#include "ast.h"


static const object_t global_true = { .type = OBJECT_BOOLEAN, .boolean = true };
static const object_t global_false = { .type = OBJECT_BOOLEAN, .boolean = false };
static const object_t global_null = { .type = OBJECT_NULL, .null = NULL};

char *object_type_to_string(object_type_t object_type){
    switch(object_type){
        case OBJECT_INTEGER:
            return "OBJECT_INTEGER";
        case OBJECT_BOOLEAN:
            return "OBJECT_BOOLEAN";
        case OBJECT_NULL:
            return "OBJECT_NULL";
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
            return expression->boolean ? global_true : global_false;
        case PREFIX_EXPR: {
            object_t right = eval(expression->prefix_expression.right, NODE_EXPRESSION);
            object_t result = eval_prefix_expression(expression->prefix_expression.op, right);
            return result;
        }
        default:
            return (object_t){};
    }
}

object_t eval_prefix_expression(char *op, object_t right){
    if(strcmp(op, "!") == 0) {
        object_t result = eval_bang_operator(right);
        return result;
    }
    if(strcmp(op, "-") == 0) {
        return eval_minus_operator(right);
    }
    return global_null;
}

object_t eval_bang_operator(object_t right){
    if (right.type == OBJECT_NULL) {
        return global_true;
    }
    else if (right.type == OBJECT_BOOLEAN && !right.boolean) {
        return global_true; 
    }
    return global_false;
}

object_t eval_minus_operator(object_t right){
    if(right.type != OBJECT_INTEGER){ return global_null; }
    int value = right.integer;
    return (object_t){
        .type = OBJECT_INTEGER,
        .integer = -value };
}

void inspect_object(object_t object, char *buff_out){
    switch(object.type){
        case OBJECT_INTEGER:
            snprintf(buff_out, BUFSIZ, "%d\n", object.integer);
            break;
        case OBJECT_BOOLEAN:
            snprintf(buff_out, BUFSIZ, "%s\n", object.boolean ? "true" : "false");
            break;
        case OBJECT_NULL:
            snprintf(buff_out, BUFSIZ, "NULL\n");
            break;
        default:
            snprintf(buff_out, BUFSIZ, "Unknown object type\n");
            break;
    }
}

