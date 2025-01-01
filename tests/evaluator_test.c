#include "test_helpers.h"
#include "../src/evaluator.h"
#include "../src/lexer.h"
#include "../src/parser.h"

void check_integer_object(object_t evaluated, int expected){
        assertf(evaluated.type == OBJECT_INTEGER,
                "wrong type, expected OBJECT_INTEGER, got %s\n",
                object_type_to_string(evaluated.integer));
        assertf(evaluated.integer == expected,
                "wrong value, expected %d, got %d\n",
                expected,
                evaluated.integer);
}

void test_eval_integer_expression(){
        struct {
                char *input;
                int expected;
        } tests[] = {
                {"5", 5},
                {"10", 10},
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		lexer_t *lexer = new_lexer(tests[i].input);
		parser_t *parser = new_parser(lexer);

		program_t *program = parse_program(parser);
                statement_t *statement = program->statements->data[0];
                object_t evaluated = eval(statement, NODE_STATEMENT);
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
        };

        for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
                lexer_t *lexer = new_lexer(tests[i].input);
                parser_t *parser = new_parser(lexer);

                program_t *program = parse_program(parser);
                statement_t *statement = program->statements->data[0];
                object_t evaluated = eval(statement, NODE_STATEMENT);
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
                statement_t *statement = program->statements->data[0];
                object_t evaluated = eval(statement, NODE_STATEMENT);
                check_boolean_object(evaluated, tests[i].expected);
        }
}
int main(int argc, char *argv[]) {
        TEST(test_eval_boolean_expression);
        TEST(test_eval_integer_expression);
        TEST(test_eval_bang_operator);
}
