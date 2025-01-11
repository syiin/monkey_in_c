#include <stdio.h>
#include <string.h>
#include "environment.h"
#include "evaluator.h"
#include "ast.h"
#include "custom_string.h"
#include "vector.h"

char *object_type_to_string(object_type_t object_type){
    switch(object_type){
        case OBJECT_INTEGER:
            return "OBJECT_INTEGER";
        case OBJECT_BOOLEAN:
            return "OBJECT_BOOLEAN";
        case OBJECT_NULL:
            return "OBJECT_NULL";
        case OBJECT_RETURN:
            return "RETURN";
        case OBJECT_ERROR:
            return "ERROR";
        default:
            return "";
    }
}

object_t eval(void *node, node_type_t node_type, environment_t *env){
    switch(node_type){
        case NODE_PROGRAM:{
            program_t *program = (program_t *)node;
            return eval_program(program, env);
        }
        case NODE_EXPRESSION:{
            expression_t *expr = (expression_t *)node;
            return eval_expression_node(expr, env);
        }
        case NODE_STATEMENT:{
            statement_t *sttmnt = (statement_t *)node;
            return eval_statement(sttmnt, env);
        }
        case NODE_BLOCK_STATEMENT:{
            block_statement_t *block_statement = (block_statement_t *)node;
            return eval_block_statement(block_statement, env);
        }
        default:
            return (object_t){};
    }
}

object_t eval_program(program_t *program, environment_t *env){
    object_t result;
    for(int i = 0; i < program->statements->count; i++){
        result = eval(program->statements->data[i], NODE_STATEMENT, env);
        if (result.type == OBJECT_RETURN || result.type == OBJECT_ERROR){
            return result;
        }
    }
    return result;
}

object_t eval_statement(statement_t *statement, environment_t *env){
    object_t result = eval(statement->value, NODE_EXPRESSION, env);
    if (result.type == OBJECT_ERROR){
        return result;
    }
    if (statement->type == RETURN_STATEMENT){
        result.type = OBJECT_RETURN;
        return result;
    } else if (statement->type == LET_STATEMENT){
        env_set(env, statement->name.value, &result);
    }
    return result;
}

object_t eval_block_statement(block_statement_t *block_statement, environment_t *env){
    object_t result;
    vector_t *statements = block_statement->statements;
    for(int i = 0; i < statements->count; i++){
         result = eval_statement(statements->data[i], env);
         if (result.type == OBJECT_RETURN || result.type == OBJECT_ERROR){
            return result;
         }
    }
    return result;
}

object_t eval_expression_node(expression_t *expression, environment_t *env){
    switch(expression->type){
        case INTEGER_LITERAL:
            return (object_t){
                .type = OBJECT_INTEGER,
                .integer = expression->integer
            };
        case BOOLEAN_EXPR:
            return native_bool_to_boolean(expression->boolean);
        case PREFIX_EXPR: {
            object_t right = eval(expression->prefix_expression.right, NODE_EXPRESSION, env);
            if(right.type == OBJECT_ERROR){ return right; }
            return eval_prefix_expression(expression->prefix_expression.op, right);
        }
        case INFIX_EXPR:{
            object_t right = eval(expression->infix_expression.right, NODE_EXPRESSION, env);
            if(right.type == OBJECT_ERROR){ return right; }

            object_t left = eval(expression->infix_expression.left, NODE_EXPRESSION, env);
            if(left.type == OBJECT_ERROR){ return left; }

            return eval_infix_expression(expression->infix_expression.op, left, right);
        }
        case IF_EXPR:{
            object_t condition = eval(expression->if_expression.condition, NODE_EXPRESSION, env);
            if(condition.type == OBJECT_ERROR){ return condition; }

            if (is_truthy(condition)){
                object_t result = eval(expression->if_expression.consequence, NODE_BLOCK_STATEMENT, env);
                if (result.type == OBJECT_ERROR){ return result; }
                return result;
            } else if (expression->if_expression.alternative != NULL){
                return eval(expression->if_expression.alternative, NODE_BLOCK_STATEMENT, env);
            } else {
                return global_null;
            }
        }
        case IDENT_EXPR:{
            object_t *value = env_get(env, expression->ident.value);
            if (value == NULL){
                return (object_t){
                    .type = OBJECT_ERROR,
                    .error_message = string_from("identifier not found")
                };
            }
            return *value;
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
    switch(op[0]){
        case '!':
            return eval_bang_operator(right);
        case '-':
            return eval_minus_operator(right);
        default:{
            char error_msg[BUFSIZ];
            snprintf(error_msg, BUFSIZ, "unknown operator: %s%s", op, object_type_to_string(right.type));
            return new_error(error_msg);
        }
    }
}

object_t eval_infix_expression(char *op, object_t left, object_t right){
    if (left.type != right.type){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "type mismatched: %s %s %s",object_type_to_string(right.type), op, object_type_to_string(right.type));
        return new_error(error_msg);
    }
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

    char error_msg[BUFSIZ];
    snprintf(error_msg, BUFSIZ, "unknown operator: %s%s%s",object_type_to_string(right.type), op, object_type_to_string(right.type));
    return new_error(error_msg);
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

    char error_msg[BUFSIZ];
    snprintf(error_msg, BUFSIZ, "unknown operator: %s %s %s",object_type_to_string(right.type), op, object_type_to_string(right.type));
    return new_error(error_msg);
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
    if(right.type != OBJECT_INTEGER){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "unknown operator: -%s ",object_type_to_string(right.type));
        return new_error(error_msg);
    }
    int value = right.integer;
    return (object_t){
        .type = OBJECT_INTEGER,
        .integer = -value };
}

object_t new_error(char *format){
    return (object_t){
        .type = OBJECT_ERROR,
        .error_message = string_from(format),
    };
}

