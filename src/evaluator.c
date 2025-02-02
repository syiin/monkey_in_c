#include <stdio.h>
#include <string.h>
#include "environment.h"
#include "evaluator.h"
#include "ast.h"
#include "custom_string.h"
#include "hashmap.h"
#include "object.h"
#include "vector.h"

char *object_type_to_string(object_type_t object_type){
    switch(object_type){
        case OBJECT_INTEGER:
            return "OBJECT_INTEGER";
        case OBJECT_STRING:
            return "OBJECT_STRING";
        case OBJECT_BOOLEAN:
            return "OBJECT_BOOLEAN";
        case OBJECT_NULL:
            return "OBJECT_NULL";
        case OBJECT_RETURN:
            return "OBJECT_RETURN";
        case OBJECT_ERROR:
            return "OBJECT_ERROR";
        case OBJECT_FUNCTION:
            return "OBJECT_FUNCTION";
        case OBJECT_BUILTIN:
            return "OBJECT_BUILTIN";
        case OBJECT_ARRAY:
            return "OBJECT_ARRAY";
        case OBJECT_HASH:
            return "OBJECT_HASH";
        default:
            return "";
    }
}

object_t *eval(void *node, node_type_t node_type, environment_t *env){
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
            return NULL;
    }
}

object_t *eval_program(program_t *program, environment_t *env){
    object_t *result;
    for(int i = 0; i < program->statements->count; i++){
        result = eval(program->statements->data[i], NODE_STATEMENT, env);
        if (result->type == OBJECT_ERROR){
            return result;
        }
        if (result->type == OBJECT_RETURN){
            return result->return_obj;
        }
    }
    return result;
}

object_t *eval_statement(statement_t *statement, environment_t *env){
    object_t *result = eval(statement->value, NODE_EXPRESSION, env);
    if (result->type == OBJECT_ERROR){
        return result;
    } else if (statement->type == RETURN_STATEMENT){
        object_t *to_return = new_object(OBJECT_RETURN);
        to_return->return_obj = result;
        return to_return;
    } else if (statement->type == LET_STATEMENT){
        env_set(env, statement->name.value, result);
    }
    return result;
}

object_t *eval_block_statement(block_statement_t *block_statement, environment_t *env){
    object_t *result;
    vector_t *statements = block_statement->statements;
    for(int i = 0; i < statements->count; i++){
         result = eval_statement(statements->data[i], env);
         if (result->type == OBJECT_RETURN || result->type == OBJECT_ERROR){
            return result;
         }
    }
    return result;
}

object_t *eval_expression_node(expression_t *expression, environment_t *env){
    switch(expression->type){
        case INTEGER_LITERAL:{
            object_t *integer_obj = new_object(OBJECT_INTEGER);
            integer_obj->integer = expression->integer;
            return integer_obj;
        }
        case BOOLEAN_EXPR:
            return native_bool_to_boolean(expression->boolean);
        case PREFIX_EXPR: {
            object_t *right = eval(expression->prefix_expression.right, NODE_EXPRESSION, env);
            if(right->type == OBJECT_ERROR){ return right; }
            return eval_prefix_expression(expression->prefix_expression.op, right);
        }
        case INFIX_EXPR:{
            object_t *right = eval(expression->infix_expression.right, NODE_EXPRESSION, env);
            if(right->type == OBJECT_ERROR){ return right; }

            object_t *left = eval(expression->infix_expression.left, NODE_EXPRESSION, env);
            if(left->type == OBJECT_ERROR){ return left; }

            return eval_infix_expression(expression->infix_expression.op, left, right);
        }
        case IF_EXPR:{
            object_t *condition = eval(expression->if_expression.condition, NODE_EXPRESSION, env);
            if(condition->type == OBJECT_ERROR){ return condition; }

            if (is_truthy(condition)){
                object_t *result = eval(expression->if_expression.consequence, NODE_BLOCK_STATEMENT, env);
                if (result->type == OBJECT_ERROR){ return result; }
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
                value = get_builtin_by_name(expression->ident.value);
            }
            if (value == NULL){
                char error_msg[BUFSIZ];
                snprintf(error_msg, BUFSIZ, "identifier not found: %s", expression->ident.value);
                return new_error(error_msg);
            }

            return value;
        }
        case FUNCTION_LITERAL: {
            object_t *obj = new_object(OBJECT_FUNCTION);
            obj->function = (function_object_t){
                    .parameters = expression->function_literal.parameters,
                    .body = expression->function_literal.body,
                    .env = env
            };
            return obj;
        }
        case CALL_EXPRESSION: {
            object_t *function = eval_expression_node(expression->call_expression.function, env);
            if (function->type == OBJECT_ERROR){ return function; }
            if (function->type != OBJECT_FUNCTION && function->type != OBJECT_BUILTIN){
                return new_error("not a function");
            }
            vector_t *args = eval_call_expressions(expression->call_expression.arguments, env);

            if (args->count == 1 && ((object_t *)args->data[0])->type == OBJECT_ERROR){
                return args->data[0];
            }
            if (function->type == OBJECT_FUNCTION && args->count != function->function.parameters->count) {
                return new_error("wrong number of arguments");
            }

            object_t *result = apply_function(function, args);

            //TODO: free the args vector
            return result;

        }
        case STRING_LITERAL: {
            object_t *obj = new_object(OBJECT_STRING);
            obj->string_literal = expression->string_literal;
            return obj;
        }
        case ARRAY_LITERAL: {
            vector_t *elements = eval_call_expressions(expression->array_literal.elements, env);
            if (elements->count == 1 && ((object_t *)elements->data[0])->type == OBJECT_ERROR){
                return elements->data[0];
            }

            object_t *obj = new_object(OBJECT_ARRAY);
            if (obj == NULL) {
                return new_error("cannot allocate for array");
            }

            obj->array.elements = elements;
            return obj;
        }
        case HASH_LITERAL: {
            object_t *obj = new_object(OBJECT_HASH);
            hash_map_t *pairs = new_hash_table(free_object);
            for (int i = 0; i < expression->hash_literal.pairs_len; i++){
                object_t *key = eval_expression_node(expression->hash_literal.pairs[i]->key, env);
                if (key->type == OBJECT_ERROR){
                    return key;
                }
                char *actual_key = object_to_key(key);
                object_t *value = eval_expression_node(expression->hash_literal.pairs[i]->value, env);
                if (value->type == OBJECT_ERROR){
                    return value;
                }
                hash_set(pairs, actual_key, value);
            }
            obj->hash.pairs = pairs;
            return obj;
        }
        case INDEX_EXPR: {
            object_t *left = eval(expression->index_expression.left, NODE_EXPRESSION, env);
            if (left->type == OBJECT_ERROR){ return left; }

            object_t *index = eval(expression->index_expression.index, NODE_EXPRESSION, env);
            if (index->type == OBJECT_ERROR){ return index; }

            return eval_index_expression(left, index);

        }
        default:
            return NULL;
    }
}

object_t *eval_index_expression(object_t *left, object_t *index){
    if (left->type == OBJECT_ARRAY && index->type == OBJECT_INTEGER){
        return eval_array_index_expression(left, index);
    }

    if (left->type == OBJECT_HASH){
        return eval_hash_index_expression(left, index);
    }

    char error_msg[BUFSIZ];
    snprintf(error_msg, BUFSIZ, "index operator not supported: %s", object_type_to_string(left->type));
    return new_error(error_msg);
}

object_t *eval_hash_index_expression(object_t *hash, object_t *index){
    char key[BUFSIZ];
    switch (index->type){
        case OBJECT_STRING:{
            snprintf(key, BUFSIZ, "OBJECT_STRING-%s", index->string_literal->data);
            break;
        }
        case OBJECT_INTEGER:{
            snprintf(key, BUFSIZ, "OBJECT_INTEGER-%d", index->integer);
            break;
        }
        case OBJECT_BOOLEAN:{
            snprintf(key, BUFSIZ, "OBJECT_BOOLEAN-%s", index->boolean ? "true" : "false");
            break;
        }
        default:{
            char error_msg[BUFSIZ];
            snprintf(error_msg, BUFSIZ, "unuseable as hash key: %s", object_type_to_string(index->type));
            return new_error(error_msg);
        }

    }
    object_t *value = hash_get(hash->hash.pairs, key);
    if (value == NULL){ return global_null; }
    return value;
}

object_t *eval_array_index_expression(object_t *array, object_t *index){
    int idx = index->integer;
    int max = array->array.elements->count;
    if (idx < 0 || idx >= max){
        return global_null;
    }
    return (object_t *)array->array.elements->data[idx];
}

vector_t *eval_call_expressions(vector_t *input_args, environment_t *env){
    vector_t *args = create_vector();

    for (int i = 0; i < input_args->count; i++){
        object_t *arg = eval_expression_node(input_args->data[i], env);

        if (arg->type == OBJECT_ERROR){ 
            append_vector(args, arg);
            return args;
        }

        append_vector(args, arg);
    }
    return args;
}

object_t *apply_function(object_t *fn, vector_t *args) {
    switch (fn->type) {
        case OBJECT_FUNCTION: {
            environment_t* extended_env = new_environment();
            extended_env->outer = fn->function.env;

            for (int i = 0; i < fn->function.parameters->count; i++) {
                identifier_t* param = fn->function.parameters->data[i];
                object_t* arg = args->data[i];
                env_set(extended_env, param->value, arg);
            }

            object_t* evaluated = eval(fn->function.body, NODE_BLOCK_STATEMENT, extended_env);
            if (evaluated->type == OBJECT_RETURN) {
                return evaluated->return_obj;
            }
            return evaluated;
        }
        case OBJECT_BUILTIN:
            return fn->builtin(args);
        default:{
            char error_msg[BUFSIZ];
            snprintf(error_msg, BUFSIZ, "not a function: %s", object_type_to_string(fn->type));
            return new_error(error_msg);
        }
    }
}

bool is_truthy(object_t *object){
    if (object->type == OBJECT_NULL) return NULL;
    return object->boolean;
}

object_t *native_bool_to_boolean(bool input){
    return input ? global_true : global_false;
}

object_t *eval_prefix_expression(char *op, object_t *right){
    switch(op[0]){
        case '!':
            return eval_bang_operator(right);
        case '-':
            return eval_minus_operator(right);
        default:{
            char error_msg[BUFSIZ];
            snprintf(error_msg, BUFSIZ, "unknown operator: %s%s", op, object_type_to_string(right->type));
            return new_error(error_msg);
        }
    }
}

object_t *eval_infix_expression(char *op, object_t *left, object_t *right){
    if (left->type != right->type){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "type mismatch: %s %s %s",object_type_to_string(left->type), op, object_type_to_string(right->type));
        return new_error(error_msg);
    } else if(left->type == OBJECT_INTEGER && right->type == OBJECT_INTEGER){
        return eval_integer_infix_expression(op, left, right);
    } else if (left->type == OBJECT_STRING && right->type == OBJECT_STRING){
        return eval_string_infix_expression(op, left, right);
    }
    // TODO: this coincidentally works for both bools and integers - does this need to be disambiguiated? are there platforms where integers and booleans are encoded differently in the union?
    if(strcmp(op, "==") == 0){
        return native_bool_to_boolean(left->integer == right->integer);
    }
    if(strcmp(op, "!=") == 0){
        return native_bool_to_boolean(left->integer != right->integer);
    }

    char error_msg[BUFSIZ];
    snprintf(error_msg, BUFSIZ, "unknown operator: %s %s %s",object_type_to_string(right->type), op, object_type_to_string(right->type));
    return new_error(error_msg);
}

object_t *eval_string_infix_expression(char *op, object_t *left, object_t *right){
    if (strcmp(op, "+") != 0){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "unknown operator: %s %s %s",object_type_to_string(right->type), op, object_type_to_string(right->type));
        return new_error(error_msg);
    }

    object_t *obj = new_object(OBJECT_STRING);
    obj->string_literal = string_concat(left->string_literal, right->string_literal);
    return obj;
}

object_t *eval_integer_infix_expression(char *op, object_t *left, object_t *right){
    int left_value = left->integer;
    int right_value = right->integer;

    if(strcmp(op, "+") == 0){
            object_t *obj = new_object(OBJECT_INTEGER);
            obj->integer = left_value + right_value;
            return obj;
    } else if(strcmp(op, "-") == 0){
            object_t *obj = new_object(OBJECT_INTEGER);
            obj->integer = left_value - right_value;
            return obj;
    } else if(strcmp(op, "*") == 0){
            object_t *obj = new_object(OBJECT_INTEGER);
            obj->integer = left_value * right_value;
            return obj;
    } else if(strcmp(op, "/") == 0){
            object_t *obj = new_object(OBJECT_INTEGER);
            obj->integer = left_value / right_value;
            return obj;
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
    snprintf(error_msg, BUFSIZ, "unknown operator: %s %s %s",object_type_to_string(right->type), op, object_type_to_string(right->type));
    return new_error(error_msg);
}

object_t *eval_bang_operator(object_t *right){
    if (right->type == OBJECT_NULL) {
        return global_true;
    }
    else if (right->type == OBJECT_BOOLEAN && !right->boolean) {
        return global_true;
    }
    return global_false;
}

object_t *eval_minus_operator(object_t *right){
    if(right->type != OBJECT_INTEGER){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "unknown operator: -%s ",object_type_to_string(right->type));
        return new_error(error_msg);
    }
    int value = right->integer;
    object_t *obj = new_object(OBJECT_INTEGER);
    obj->integer = -value;
    return obj;
}

object_t *new_error(char *format){
    object_t *obj = new_object(OBJECT_ERROR);
    obj->error_message = string_from(format);
    return obj;
}
