#include "test_helpers.h"
#include "../src/evaluator.h"
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/environment.h"

void check_integer_object(object_t evaluated, int expected){
        assertf(evaluated.type == OBJECT_INTEGER,
                "wrong type, expected OBJECT_INTEGER, got %s\n",
                object_type_to_string(evaluated.integer));
        assertf(evaluated.integer == expected,
                "wrong value, expected %d, got %d\n",
                expected,
                evaluated.integer);
}

void check_return_integer(object_t evaluated, int expected){
        assertf(evaluated.type == OBJECT_RETURN,
                "wrong type, expected OBJECT_INTEGER, got %s\n",
                object_type_to_string(evaluated.integer));
        assertf(evaluated.integer == expected,
                "wrong value, expected %d, got %d\n",
                expected,
                evaluated.integer);
}


void check_error(object_t evaluated, char *expected_msg){
        assertf(evaluated.type == OBJECT_ERROR,
                "wrong type, expected OBJECT_ERROR, got %s\n",
                object_type_to_string(evaluated.integer));
        char *error_msg = string_get_data(evaluated.error_message);
        assertf(strcmp(error_msg, expected_msg) != 0,
               "wrong error message, expected %s, got %s\n",
               expected_msg,
               error_msg);
}

void test_eval_integer_expression(){
        struct {
                char *input;
                int expected;
        } tests[] = {
                {"5", 5},
                {"10", 10},
                {"5", 5},
                {"10", 10},
                {"-5", -5},
                {"-10", -10},
                {"5 + 5 + 5 + 5 - 10", 10},
                {"2 * 2 * 2 * 2 * 2", 32},
                {"-50 + 100 + -50", 0},
                {"5 * 2 + 10", 20},
                {"5 + 2 * 10", 25},
                {"20 + 2 * -10", 0},
                {"50 / 2 * 2 + 10", 60},
                {"2 * (5 + 10)", 30},
                {"3 * 3 * 3 + 10", 37},
                {"3 * (3 * 3) + 10", 37},
                {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		lexer_t *lexer = new_lexer(tests[i].input);
		parser_t *parser = new_parser(lexer);

		program_t *program = parse_program(parser);
		environment_t *env = new_environment();
                object_t evaluated = eval(program, NODE_PROGRAM, env);
                check_integer_object(evaluated, tests[i].expected);

        }
}

void check_boolean_object(object_t evaluated, bool expected){
        assertf(evaluated.type == OBJECT_BOOLEAN,
                "wrong type, expected OBJECT_BOOLEAN, got %s\n",
                object_type_to_string(evaluated.type));
        assertf(evaluated.boolean == expected,
                "wrong value, expected %d, got %d\n",
                expected,
                evaluated.boolean);
}

void test_eval_boolean_expression(){
        struct {
                char *input;
                bool expected;
        } tests[] = {
                {"true", true},
                {"false", false},
                {"1 < 2", true},
                {"1 > 2", false},
                {"1 < 1", false},
                {"1 > 1", false},
                {"1 == 1", true},
                {"1 != 1", false},
                {"1 == 2", false},
                {"1 != 2", true},
                {"true == true", true},
                {"false == false", true},
                {"true == false", false},
                {"true != false", true},
                {"false != true", true},
                {"(1 < 2) == true", true},
                {"(1 < 2) == false", false},
                {"(1 > 2) == true", false},
                {"(1 > 2) == false", true},
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);

                program_t *program = parse_program(parser);
		environment_t *env = new_environment();
                statement_t *statement = program->statements->data[0];
                object_t evaluated = eval(statement, NODE_STATEMENT, env);
                check_boolean_object(evaluated, tests[i].expected);
        }
}

void test_eval_bang_operator(){
        struct {
                char *input;
                bool expected;
        } tests[] = {
                {"!true", false},
                {"!false", true},
                {"!5", false},
                {"!!true", true},
                {"!!false", false},
                {"!!5", true},
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);

                program_t *program = parse_program(parser);
		environment_t *env = new_environment();

                object_t evaluated = eval(program, NODE_PROGRAM, env);
                check_boolean_object(evaluated, tests[i].expected);
        }
}

bool check_null_object(object_t obj) {
    if (obj.type != OBJECT_NULL) {
        printf("Error: object is not NULL. got=%s \n", 
               object_type_to_string(obj.type));
        return false;
    }
    return true;
}

void test_eval_if_else_expressions() {
        struct {
                char *input;
                int expected;
                bool has_value;  // to differentiate between null and integer cases
        } tests[] = {
                {"if (true) { 10 }", 10, true},
                {"if (false) { 10 }", 0, false},  // null case
                {"if (1) { 10 }", 10, true},
                {"if (1 < 2) { 10 }", 10, true},
                {"if (1 > 2) { 10 }", 0, false},  // null case
                {"if (1 > 2) { 10 } else { 20 }", 20, true},
                {"if (1 < 2) { 10 } else { 20 }", 10, true},
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
		environment_t *env = new_environment();
                object_t evaluated = eval(program, NODE_PROGRAM, env);
                if (tests[i].has_value) {
                        check_integer_object(evaluated, tests[i].expected);
                } else {
                        check_null_object(evaluated);
                }
        }
}

void test_eval_return_statements() {
        struct {
                char *input;
                int expected;
        } tests[] = {
                {"return 10;", 10},
                {"return 10; 9;", 10},
                {"return 2 * 5; 9;", 10},
                {"9; return 2 * 5; 9;", 10},
                {"if (10 > 1) { \
                        if (10 > 1) { \
                                return 10; \
                        } \
                        return 1; \
                }", 10},
                };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
		environment_t *env = new_environment();
                object_t evaluated = eval(program, NODE_PROGRAM, env);
                check_return_integer(evaluated, tests[i].expected);
        }
}

void test_eval_error_handling() {
        struct {
                char *input;
                char *expected_message;
        } tests[] = {
                {
                   "5 + true;",
                   "type mismatch: OBJECT_INTEGER + OBJECT_BOOLEAN"
                },
                {
                   "5 + true; 5;", 
                   "type mismatch: OBJECT_INTEGER + OBJECT_BOOLEAN"
                },
                {
                   "-true",
                   "unknown operator: -OBJECT_BOOLEAN"
                },
                {
                   "true + false;",
                   "unknown operator: OBJECT_BOOLEAN + OBJECT_BOOLEAN"
                },
                {
                   "5; true + false; 5",
                   "unknown operator: OBJECT_BOOLEAN + OBJECT_BOOLEAN"
                },
                {
                   "if (10 > 1) { true + false; }",
                   "unknown operator: OBJECT_BOOLEAN + OBJECT_BOOLEAN"
                },
                {
                   "if (10 > 1) {\n"
                   "  if (10 > 1) {\n"
                   "    return true + false;\n"
                   "  }\n"
                   "  return 1;\n"
                   "}",
                   "unknown operator: OBJECT_BOOLEAN + OBJECT_BOOLEAN"
                },
                {
                "foobar",
                "identifier not found: foobar",
                },
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
		environment_t *env = new_environment();
                object_t evaluated = eval(program, NODE_PROGRAM, env);
                check_error(evaluated, tests[i].expected_message);
        }
}

void test_eval_let_statements() {
        struct {
                char *input;
                int expected;
        } tests[] = {
               {"let a = 5; a;", 5},
               {"let a = 5 * 5; a;", 25},
               {"let a = 5; let b = a; b;", 5},
               {"let a = 5; let b = a; let c = a + b + 5; c;", 15}
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
               lexer_t *lexer = new_lexer(tests[i].input);
               parser_t *parser = new_parser(lexer);
               program_t *program = parse_program(parser);
               environment_t *env = new_environment();
               object_t evaluated = eval(program, NODE_PROGRAM, env);
               check_integer_object(evaluated, tests[i].expected);
        }
}

void test_eval_function_object() {
        char *input = "fn(x) { x + 2; };";

        lexer_t *lexer = new_lexer(input);
        parser_t *parser = new_parser(lexer);
        program_t *program = parse_program(parser);
        environment_t *env = new_environment();

        object_t evaluated = eval(program, NODE_PROGRAM, env);

        assertf(evaluated.type == OBJECT_FUNCTION, 
                "object is not Function. got=%s",
                object_type_to_string(evaluated.type));

        assertf(evaluated.function.parameters->count == 1,
                "function has wrong parameters. got=%d", 
                evaluated.function.parameters->count);

        identifier_t *param = evaluated.function.parameters->data[0];
        assertf(strcmp(param->value, "x") == 0,
                "parameter is not 'x'. got=%s", 
                param->value);

        string_t *body_str = string_new();
        format_block_statement(body_str, evaluated.function.body);
        assertf(strcmp(string_get_data(body_str), "(x + 2)") == 0,
                "body is not '(x + 2)'. got=%s",
                body_str);
}

int main(int argc, char *argv[]) {
        TEST(test_eval_boolean_expression);
        TEST(test_eval_integer_expression);
        TEST(test_eval_bang_operator);
        TEST(test_eval_if_else_expressions);
        TEST(test_eval_return_statements);
        TEST(test_eval_error_handling);
        TEST(test_eval_let_statements);
        TEST(test_eval_function_object);
}
