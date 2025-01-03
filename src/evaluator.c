#include <stdio.h>
#include <string.h>
#include "evaluator.h"
#include "ast.h"

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
            return eval_statements(program->statements);
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

object_t eval_statements(vector_t *statements){
    object_t result;
    for(int i = 0; i < statements->count; i++){
         result = eval(statements->data[i], NODE_STATEMENT);
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
            return native_bool_to_boolean(expression->boolean);
        case PREFIX_EXPR: {
            object_t right = eval(expression->prefix_expression.right, NODE_EXPRESSION);
            return eval_prefix_expression(expression->prefix_expression.op, right);
        }
        case INFIX_EXPR:{
            object_t right = eval(expression->infix_expression.right, NODE_EXPRESSION);
            object_t left = eval(expression->infix_expression.left, NODE_EXPRESSION);
            return eval_infix_expression(expression->infix_expression.op, left, right);
        }
        case IF_EXPR:{
            object_t condition = eval(expression->if_expression.condition, NODE_EXPRESSION);

            if (is_truthy(condition)){
                return eval_statements(expression->if_expression.consequence->statements);
            } else if (expression->if_expression.alternative != NULL){
                return eval_statements(expression->if_expression.alternative->statements);
            } else {
                return global_null;
            }
        }
        default:
            return (object_t){};
    }
}

bool is_truthy(object_t object){
    if (object.type == OBJECT_NULL) return NULL;
    return object.boolean;
}

object_t native_bool_to_boolean(bool input){
    return input ? global_true : global_false;
}

object_t eval_prefix_expression(char *op, object_t right){
    if(strcmp(op, "!") == 0) {
        return eval_bang_operator(right);
    }
    if(strcmp(op, "-") == 0) {
        return eval_minus_operator(right);
    }
    return global_null;
}

object_t eval_infix_expression(char *op, object_t left, object_t right){
    if(left.type == OBJECT_INTEGER && right.type == OBJECT_INTEGER){
        return eval_integer_infix_expression(op, left, right);
    }
    // TODO: this coincidentally works for both bools and integers - does this need to be disambiguiated? are there platforms where integers and booleans are encoded differently in the union?
    if(strcmp(op, "==") == 0){
        return native_bool_to_boolean(left.integer == right.integer);
    }
    if(strcmp(op, "!=") == 0){
        return native_bool_to_boolean(left.integer != right.integer);
    }
    return global_null;

}
object_t eval_integer_infix_expression(char *op, object_t left, object_t right){
    int left_value = left.integer;
    int right_value = right.integer;

    if(strcmp(op, "+") == 0){
            return (object_t){
                .type = OBJECT_INTEGER,
                .integer = left_value + right_value
            };
    } else if(strcmp(op, "-") == 0){
            return (object_t){
                .type = OBJECT_INTEGER,
                .integer = left_value - right_value
            };
    } else if(strcmp(op, "*") == 0){
            return (object_t){
                .type = OBJECT_INTEGER,
                .integer = left_value * right_value
            };
    } else if(strcmp(op, "/") == 0){
            return (object_t){
                .type = OBJECT_INTEGER,
                .integer = left_value / right_value
            };
    } else if(strcmp(op, "<") == 0){
            return native_bool_to_boolean(left_value < right_value);
    } else if(strcmp(op, ">") == 0){
            return native_bool_to_boolean(left_value > right_value);
    } else if(strcmp(op, "==") == 0){
            return native_bool_to_boolean(left_value == right_value);
    } else if(strcmp(op, "!=") == 0){
            return native_bool_to_boolean(left_value != right_value);
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

