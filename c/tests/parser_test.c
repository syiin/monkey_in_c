#include "test_helpers.h"
#include "../src/lexer.h"
#include "../src/parser.h"
#include <string.h>

void check_parser_errors(parser_t *parser){
	if (parser->errors->count > 0){
		print_errors(parser);
	}
	assertf(parser->errors->count == 0, "expected no errors but got %d", 0, parser->errors->count);
}

void check_integer_literal(expression_t *expression, int value){
	assertf(expression->integer == value, "wrong value. expected %d, got %d\n", value, expression->integer);
	char literal_buffer[32];
	sprintf(literal_buffer, "%d", value);
	assertf(strcmp(expression->token.literal, literal_buffer) == 0, "wrong literal. expected %s, got %s\n", literal_buffer, expression->token.literal);
}

void check_identifier(expression_t *expression, char *value) {
	assertf(expression->type == IDENT_EXPR, "exp not IDENT_EXPR. got=%d\n", expression->type);
	assertf(strcmp(expression->ident.value, value) == 0, "ident.Value not %s. got=%s\n", value, expression->ident.value);
	assertf(strcmp(expression->token.literal, value) == 0, "ident.TokenLiteral not %s. got=%s\n", value, expression->token.literal);
}

void check_boolean_literal(expression_t *expression, bool value) {
    assertf(expression->type == BOOLEAN_EXPR, 
            "exp not BOOLEAN_EXPR. got=%d", 
            expression->type);

    assertf(expression->boolean == value, 
            "boolean.Value not %d. got=%d", 
            value, 
            expression->boolean);

    char literal_buffer[6];  // enough for "true" or "false"
    sprintf(literal_buffer, "%s", value ? "true" : "false");
    assertf(strcmp(expression->token.literal, literal_buffer) == 0,
            "wrong literal. expected %s, got %s", 
            literal_buffer, 
            expression->token.literal);
}

void test_let_statements(){
	char *input = "let x = 5;\n"
		"let y = 10;\n"
		"let foobar = 838383;\n";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);

	program_t *program = parse_program(parser);
	check_parser_errors(parser);

	if (program == NULL){
		printf("Parse program failed \n");
		exit(1);
	}

	assertf(program->count == 3, "Program does not container 3 statements");
	
	struct {
		char *literal;
		char *name;
		} tests[] = {
		{"let", "x" },
		{"let", "y"},
		{"let", "foobar"},
	};
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		statement_t statement = *program->statements[i];
		assertf(statement.token.type == LET, "wrong token type. expected %s, got %s\n", "RETURN", token_type_to_string(statement.token.type));
		assertf(strcmp(statement.token.literal, tests[i].literal) == 0, "wrong literal. expected %s, got %s\n", tests[i].name, statement.token.literal);
		assertf(strcmp(statement.name.value, tests[i].name) == 0, "wrong name value. expected %s, got %s\n", tests[i].name, statement.name.value);
	}
}

void test_return_statements(){
	char *input =
		"return 5;"
		"return 10;"
		"return 993322;";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);

	program_t *program = parse_program(parser);
	check_parser_errors(parser);

	if (program == NULL){
		printf("Parse program failed \n");
		exit(1);
	}

	assertf(program->count == 3, "Program does not container 3 statements");
	
	struct {
		char *literal;
		char *name;
		} tests[] = {
		{"return", "5" },
		{"return", "10"},
		{"return", "993322"},
	};
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		statement_t statement = *program->statements[i];
		assertf(statement.token.type == RETURN, "wrong token type. expected %s, got %s\n", "RETURN", token_type_to_string(statement.token.type));
		assertf(strcmp(statement.token.literal, tests[i].literal) == 0, "wrong literal. expected %s, got %s\n", tests[i].name, statement.token.literal);
	}
}

void test_identifier(){
	char *input = "foobar;";

	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);

	program_t *program = parse_program(parser);
	check_parser_errors(parser);

	if (program == NULL){
		printf("Parse program failed \n");
		exit(1);
	}
	assertf(program->count == 1, "Program does not container 1 statements");

	statement_t statement = *program->statements[0];

	assertf(statement.type == EXPRESSION_STATEMENT, "wrong token type. expected %s, got %s\n", "EXPRESSION_STATEMENT", token_type_to_string(statement.token.type));
	assertf(statement.value->type == IDENT_EXPR, "wrong expression type. expected %s, got %d\n", "IDENT_EXPR", statement.value->type);
	assertf(strcmp(statement.value->ident.value, "foobar") == 0, "wrong identifier value. expected %s, got %s\n", "foobar", statement.value->ident.value);
	assertf(strcmp(statement.value->ident.token.literal, "foobar") == 0, "wrong literal. expected %s, got %s\n", "foobar", statement.token.literal);
}

void test_integer(){
	char *input = "5;";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);

	program_t *program = parse_program(parser);
	check_parser_errors(parser);

	if (program == NULL){
		printf("Parse program failed \n");
		exit(1);
	}
	assertf(program->count == 1, "Program does not container 1 statement");
	statement_t statement = *program->statements[0];

	assertf(statement.type == EXPRESSION_STATEMENT, "wrong token type. expected %s, got %s\n", "EXPRESSION_STATEMENT", token_type_to_string(statement.token.type));
	assertf(statement.value->type == INTEGER_LITERAL, "wrong expression type. expected %s, got %d\n", "INTEGER_LITERAL", statement.value->type);
	check_integer_literal(statement.value, 5);
}

void test_parsing_prefix_expressions(){
	struct {
		char	*input;
		char	*operator;
		int	value;
	} tests[] = {
		{"!5;", "!", 5},
		{"-15;", "-", 15},
		{"!true;", "!", true},
		{"!false;", "!", false},
	};
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		lexer_t *lexer = new_lexer(tests[i].input);
		parser_t *parser = new_parser(lexer);

		program_t *program = parse_program(parser);
		check_parser_errors(parser);

		if (program == NULL){
			printf("Parse program failed \n");
			exit(1);
		}
		assertf(program->count == 1, "Program does not container 1 statement");
		statement_t statement = *program->statements[0];

		assertf(statement.type == EXPRESSION_STATEMENT, "wrong token type. expected %s, got %s\n", "EXPRESSION_STATEMENT", token_type_to_string(statement.token.type));
		assertf(statement.value->type == PREFIX_EXPR, "wrong expression type. expected %s, got %d\n", "PREFIX_EXPRESSION", statement.value->type);
		assertf(strcmp(statement.value->prefix_expression.op, tests[i].operator) == 0, "wrong operator type. got %s, expected %s\n", statement.value->prefix_expression.op, tests[i].operator);

		if (statement.value->infix_expression.right->token.type == INT){
			check_integer_literal(statement.value->prefix_expression.right, tests[i].value);
		}

		if (statement.value->infix_expression.right->token.type == TRUE || 
				statement.value->infix_expression.right->token.type == FALSE){
			check_boolean_literal(statement.value->infix_expression.right, tests[i].value);
		}
	}
}

void test_parsing_infix_expressions() {
	struct {
		char *input;
		int left_value;
		char *operator;
		int right_value;
	} tests[] = {
		{"5 + 5;", 5, "+", 5},
		{"5 - 5;", 5, "-", 5},
		{"5 * 5;", 5, "*", 5},
		{"5 / 5;", 5, "/", 5},
		{"5 > 5;", 5, ">", 5},
		{"5 < 5;", 5, "<", 5},
		{"5 == 5;", 5, "==", 5},
		{"5 != 5;", 5, "!=", 5},
		{"true == true;", true, "==", true},
		{"true != false;", true, "!=", false},
		{"false == false;", false, "==" , false},
	};
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
		lexer_t *lexer = new_lexer(tests[i].input);
		parser_t *parser = new_parser(lexer);

		program_t *program = parse_program(parser);
		check_parser_errors(parser);

		assertf(program->count == 1, 
		       "program.Statements does not contain 1 statement. got=%d\n",
		       program->count);

		statement_t *stmt = program->statements[0];
		assertf(stmt->type == EXPRESSION_STATEMENT,
		       "program.Statements[0] is not EXPRESSION_STATEMENT. got=%d",
		       stmt->type);

		expression_t *exp = stmt->value;
		assertf(exp->type == INFIX_EXPR,
		       "exp is not INFIX_EXPRESSION. got=%d",
		       exp->type);

		if (exp->infix_expression.left->token.type == INT){
			check_integer_literal(exp->infix_expression.left, tests[i].left_value);
			assertf(strcmp(exp->infix_expression.op, tests[i].operator) == 0,
			       "exp.Operator is not '%s'. got=%s",
			       tests[i].operator,
			       exp->infix_expression.op);
			check_integer_literal(exp->infix_expression.right, tests[i].right_value);
		}

		if (exp->infix_expression.left->token.type == TRUE || 
				exp->infix_expression.left->token.type == FALSE){
			check_boolean_literal(exp->infix_expression.left, tests[i].left_value);
			assertf(strcmp(exp->infix_expression.op, tests[i].operator) == 0,
			       "exp.Operator is not '%s'. got=%s",
			       tests[i].operator,
			       exp->infix_expression.op);
			check_boolean_literal(exp->infix_expression.right, tests[i].right_value);
		}
    }
}

void test_operator_precedence() {
	struct {
		char *input;
		char *expected;
	} tests[] = {
		{"-a * b;", "((-a) * b)"},
		{"!-a;", "(!(-a))"},
		{"a + b + c;", "((a + b) + c)"},
		{"a + b - c;", "((a + b) - c)"},
		{"a * b * c;", "((a * b) * c)"},
		{"a * b / c;", "((a * b) / c)"},
		{"a + b / c;", "(a + (b / c))"},
		{"a + b * c + d / e - f;", "(((a + (b * c)) + (d / e)) - f)"},
		{"3 + 4; -5 * 5;", "(3 + 4)((-5) * 5)"},
		{"5 > 4 == 3 < 4;", "((5 > 4) == (3 < 4))"},
		{"5 < 4 != 3 > 4;", "((5 < 4) != (3 > 4))"},
		{"3 + 4 * 5 == 3 * 1 + 4 * 5;", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
		{"true;", "true"},
		{"false;", "false"},
		{"3 > 5 == false;", "((3 > 5) == false)"},
		{"3 < 5 == true;", "((3 < 5) == true)"},
		{"1 + (2 + 3) + 4;", "((1 + (2 + 3)) + 4)"},
		{"(5 + 5) * 2;", "((5 + 5) * 2)"},
		{"2 / (5 + 5);", "(2 / (5 + 5))"},
		{"(5 + 5) * 2 * (5 + 5);", "(((5 + 5) * 2) * (5 + 5))"},
		{"-(5 + 5);", "(-(5 + 5))"},
		{"!(true == true);", "(!(true == true))"},
	};

	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
		lexer_t *lexer = new_lexer(tests[i].input);
		parser_t *parser = new_parser(lexer);

		program_t *program = parse_program(parser);
		check_parser_errors(parser);

		char *actual = program_to_string(program);
		assertf(strcmp(actual, tests[i].expected) == 0, 
		       "expected=%s, got=%s", 
		       tests[i].expected, 
		       actual);
		
		free(actual);
	}
}

void test_boolean_expression() {
	struct {
		char *input;
		bool expected_boolean;
	} tests[] = {
		{"true;", true},
		{"false;", false},
	};
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
		lexer_t *lexer = new_lexer(tests[i].input);
		parser_t *parser = new_parser(lexer);

		program_t *program = parse_program(parser);
		check_parser_errors(parser);

		assertf(program->count == 1, 
		       "program has not enough statements. got=%d",
		       program->count);

		statement_t *stmt = program->statements[0];
		assertf(stmt->type == EXPRESSION_STATEMENT,
		       "program.Statements[0] is not EXPRESSION_STATEMENT. got=%d",
		       stmt->type);
		
		check_boolean_literal(stmt->value, tests[i].expected_boolean);
	}
}

void test_if_expression(void) {
	char *input = "if (x < y) { x }";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);
	program_t *program = parse_program(parser);

	check_parser_errors(parser);

	assertf(program->count == 1, 
	"program does not contain 1 statement. got=%d\n",
	program->count);

	statement_t *stmt = program->statements[0];
	assertf(stmt->type == EXPRESSION_STATEMENT,
	"statement is not expression statement. got=%d",
	stmt->type);

	expression_t *exp = stmt->value; 
	assertf(exp->type == IF_EXPR,
	"expression is not if expression. got=%d", 
	exp->type);

	expression_t *condition = exp->if_expression.condition;
	assertf(condition->type == INFIX_EXPR,
		"condition is not infix expression. got=%d",
		condition->type);
	check_identifier(condition->infix_expression.left, "x");
	assertf(strcmp(condition->infix_expression.op, "<") == 0,
		"condition operator is not '<'. got=%s",
		condition->infix_expression.op);
	check_identifier(condition->infix_expression.right, "y");

	block_statement_t *consequence = exp->if_expression.consequence;
	assertf(consequence->statements->count == 1,
		"consequence does not contain 1 statement. got=%d",
		consequence->statements->count);

	statement_t *consequence_stmt = consequence->statements->data[0];
	assertf(consequence_stmt->type == EXPRESSION_STATEMENT,
		"consequence statement is not expression statement. got=%d",
		consequence_stmt->type);

	check_identifier(consequence_stmt->value, "x");

	assertf(exp->if_expression.alternative == NULL,
	"alternative was not NULL");
}

void test_if_else_expression(void) {
	char *input = "if (x < y) { x } else { y }";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);
	program_t *program = parse_program(parser);

	check_parser_errors(parser);

	assertf(program->count == 1,
		"program does not contain 1 statement. got=%d\n",
		program->count);

	statement_t *stmt = program->statements[0];
	assertf(stmt->type == EXPRESSION_STATEMENT,
		"statement is not expression statement. got=%d",
		stmt->type);

	expression_t *exp = stmt->value;
	assertf(exp->type == IF_EXPR,
		"expression is not if expression. got=%d",
		exp->type);

	// Test condition
	expression_t *condition = exp->if_expression.condition;
	assertf(condition->type == INFIX_EXPR,
	"condition is not infix expression. got=%d",
	condition->type);
	check_identifier(condition->infix_expression.left, "x");
	assertf(strcmp(condition->infix_expression.op, "<") == 0,
		"condition operator is not '<'. got=%s",
		condition->infix_expression.op);
	check_identifier(condition->infix_expression.right, "y");

	// Test consequence
	block_statement_t *consequence = exp->if_expression.consequence;
	assertf(consequence->statements->count == 1,
		"consequence does not contain 1 statement. got=%d",
		consequence->statements->count);

	statement_t *consequence_stmt = consequence->statements->data[0];
	assertf(consequence_stmt->type == EXPRESSION_STATEMENT,
		"consequence statement is not expression statement. got=%d",
		consequence_stmt->type);
	check_identifier(consequence_stmt->value, "x");

	block_statement_t *alternative = exp->if_expression.alternative;
	assertf(alternative->statements->count == 1,
		"alternative does not contain 1 statement. got=%d\n",
		alternative->statements->count);

	statement_t *alternative_stmt = alternative->statements->data[0];
	assertf(alternative_stmt->type == EXPRESSION_STATEMENT,
		"alternative statement is not expression statement. got=%d",
		alternative_stmt->type);
	check_identifier(alternative_stmt->value, "y");
}

int main(int argc, char *argv[]) {
	TEST(test_let_statements);
	TEST(test_return_statements);
	TEST(test_identifier);
	TEST(test_integer);
	TEST(test_parsing_prefix_expressions);
	TEST(test_parsing_infix_expressions);
	TEST(test_operator_precedence);
	TEST(test_boolean_expression);
	TEST(test_if_expression);
	TEST(test_if_else_expression);
}
