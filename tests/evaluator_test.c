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
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                check_integer_object(*evaluated, tests[i].expected);

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
                object_t *evaluated = eval(statement, NODE_STATEMENT, env);
                check_boolean_object(*evaluated, tests[i].expected);
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

                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                check_boolean_object(*evaluated, tests[i].expected);
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
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                if (tests[i].has_value) {
                        check_integer_object(*evaluated, tests[i].expected);
                } else {
                        check_null_object(*evaluated);
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
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                check_integer_object(*evaluated, tests[i].expected);
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
                {
                "\"Hello\" - \"World\"",
                "unknown operator: STRING - STRING",
                },
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
		environment_t *env = new_environment();
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                check_error(*evaluated, tests[i].expected_message);
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
               object_t *evaluated = eval(program, NODE_PROGRAM, env);
               check_integer_object(*evaluated, tests[i].expected);
        }
}

void test_eval_function_object() {
        char *input = "fn(x) { x + 2; };";

        lexer_t *lexer = new_lexer(input);
        parser_t *parser = new_parser(lexer);
        program_t *program = parse_program(parser);
        environment_t *env = new_environment();

        object_t *evaluated = eval(program, NODE_PROGRAM, env);

        assertf(evaluated->type == OBJECT_FUNCTION, 
                "object is not Function. got=%s",
                object_type_to_string(evaluated->type));

        assertf(evaluated->function.parameters->count == 1,
                "function has wrong parameters. got=%d", 
                evaluated->function.parameters->count);

        identifier_t *param = evaluated->function.parameters->data[0];
        assertf(strcmp(param->value, "x") == 0,
                "parameter is not 'x'. got=%s", 
                param->value);

        string_t *body_str = string_new();
        format_block_statement(body_str, evaluated->function.body);
        assertf(strcmp(string_get_data(body_str), "(x + 2)") == 0,
                "body is not '(x + 2)'. got=%s",
                body_str);
}

void test_eval_function_application() {
        struct {
                char *input;
                int expected;
        } tests[] = {
                {"let identity = fn(x) { x; }; identity(5);", 5},
                {"let identity = fn(x) { return x; }; identity(5);", 5},
                {"let double = fn(x) { x * 2; }; double(5);", 10},
                {"let add = fn(x, y) { x + y; }; add(5, 5);", 10}, 
                {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
                {"fn(x) { x; }(5)", 5},
                {"let multiply = fn(x, y) { x * y }; multiply(2,3)", 6},
                {"let multiply = fn(x, y) { x * y }; multiply(50 / 2, 1 * 2)", 50}
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
                environment_t *env = new_environment();
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                check_integer_object(*evaluated, tests[i].expected);
        }
}

void test_closures() {
        struct {
                char *input;
                int expected;
        } tests[] = {
                {
                    "let newAdder = fn(x) {\n"
                    "   fn(y) { x + y };\n"
                    "};\n"
                    "let addTwo = newAdder(2);\n"
                    "addTwo(2);",
                    4
                }
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
                environment_t *env = new_environment();
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                check_integer_object(*evaluated, tests[i].expected);
        }
}

void test_string_literal() {
        struct {
                char *input;
                char *expected;
        } tests[] = {
                {
                    "\"Hello World!\"",
                    "Hello World!"
                }
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input); 
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
                environment_t *env = new_environment();
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                assertf(evaluated->type == OBJECT_STRING,
                        "object is not String. got=%d (%s)", 
                        evaluated->type, evaluated->string_literal->data);

                assertf(strcmp(evaluated->string_literal->data, tests[i].expected) == 0,
                        "String has wrong value. got=%s",
                        evaluated->string_literal->data);
                        }
}

void test_string_concatenation() {
        struct {
                char *input;
                char *expected; 
        } tests[] = {
                {
                    "\"Hello\" + \" \" + \"World!\"",
                    "Hello World!"
                }
        };
        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
                environment_t *env = new_environment();
                object_t *evaluated = eval(program, NODE_PROGRAM, env);
                assertf(evaluated->type == OBJECT_STRING,
                        "object is not String. got=%d (%s)",
                        evaluated->type, evaluated->string_literal->data);

                assertf(strcmp(evaluated->string_literal->data, tests[i].expected) == 0,
                        "String has wrong value. got=%s",
                        evaluated->string_literal->data);
        }
}

void test_builtin_functions() {
        struct {
                char *input;
                union {
                    int int_val;
                    char *str_val;
                } expected;
                int is_error; // Flag to indicate if expecting error message
        } tests[] = {
                {
                    "len(\"\")", 
                    {.int_val = 0},
                    0
                },
                {
                    "len(\"four\")",
                    {.int_val = 4},
                    0
                },
                {
                    "len(\"hello world\")",
                    {.int_val = 11},
                    0
                },
                {
                    "len(1)",
                    {.str_val = "argument to `len` not supported, got INTEGER"},
                    1
                },
                {
                    "len(\"one\", \"two\")",
                    {.str_val = "wrong number of arguments. got=2, want=1"},
                    1
                }
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);
                program_t *program = parse_program(parser);
                environment_t *env = new_environment();
                object_t *evaluated = eval(program, NODE_PROGRAM, env);

                if (!tests[i].is_error) {
                        // Expecting integer result
                        assertf(evaluated->type == OBJECT_INTEGER, 
                                "object is not Integer. got=%d", 
                                evaluated->type);
                        assertf(evaluated->integer == tests[i].expected.int_val,
                                "wrong integer value. got=%d, want=%d",
                                evaluated->integer, tests[i].expected.int_val);
                } else {
                        // Expecting error message 
                        assertf(evaluated->type == OBJECT_ERROR,
                                "object is not Error. got=%d",
                                evaluated->type);
                        char *error_msg = string_get_data(evaluated->error_message);
                        assertf(strcmp(error_msg, tests[i].expected.str_val) != 0,
                                "wrong error message. got=%s, want=%s",
                                error_msg, tests[i].expected.str_val);
                }
        }
}

void test_array_literals() {
        char *input = "[1, 2 * 2, 3 + 3]";

        lexer_t *lexer = new_lexer(input);
        parser_t *parser = new_parser(lexer);
        program_t *program = parse_program(parser);
        environment_t *env = new_environment();

        object_t *evaluated = eval(program, NODE_PROGRAM, env);
        printf("%s\n", object_type_to_string(evaluated->type));

        assertf(evaluated->type == OBJECT_ARRAY,
                    "object is not Array. got=%s\n",
                    object_type_to_string(evaluated->type));
        assertf(evaluated->array.elements->count == 3,
                    "array has wrong num of elements. got=%d\n",
                    evaluated->array.elements->count);
        // Test each element
        object_t *first = evaluated->array.elements->data[0];
        check_integer_object(*first, 1);

        object_t *second = evaluated->array.elements->data[1];
        check_integer_object(*second, 4);

        object_t *third = evaluated->array.elements->data[2];
        check_integer_object(*third, 6);
}


void test_array_index_expressions() {
    struct test_case {
        char *input;
        union {
            int int_val;
            object_type_t type;
        } expected;
        int should_be_null;
    };

    struct test_case tests[] = {
        {"[1, 2, 3][0]", {.int_val = 1}, false},
        {"[1, 2, 3][1]", {.int_val = 2}, false},
        {"[1, 2, 3][2]", {.int_val = 3}, false},
        {"let i = 0; [1][i];", {.int_val = 1}, false},
        {"[1, 2, 3][1 + 1];", {.int_val = 3}, false},
        {"let myArray = [1, 2, 3]; myArray[2];", {.int_val = 3}, false},
        {"let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", {.int_val = 6}, false},
        {"let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]", {.int_val = 2}, false},
        /*{"[1, 2, 3][3]", {0}, true},*/
        {"[1, 2, 3][-1]", {0}, true}
    };

    for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        lexer_t *lexer = new_lexer(tests[i].input);
        parser_t *parser = new_parser(lexer);
        program_t *program = parse_program(parser);
        environment_t *env = new_environment();
        object_t *evaluated = eval(program, NODE_PROGRAM, env);
        if (!tests[i].should_be_null){
                check_integer_object(*evaluated, tests[i].expected.int_val);
        } else {
                check_null_object(*evaluated);
        }
    }
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
        TEST(test_eval_function_application);
        TEST(test_string_literal);
        TEST(test_string_concatenation);
        TEST(test_builtin_functions);
        TEST(test_array_literals);
        TEST(test_array_index_expressions);
}
