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
	char literal_buffer[BUFSIZ];
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

void test_let_statements() {
	typedef union {
		char *ident;
		int integer;
		bool boolean;
	} value_t;

	struct {
		char *input;
		expression_type_t exp_type;
		value_t value;
		char *name;
	} tests[] = {
		{"let x = 5;", INTEGER_LITERAL, {.integer = 5}, "x"},
		{"let y = true;", BOOLEAN_EXPR, {.boolean = true}, "y"},
		{"let foobar = y;", IDENT_EXPR, {.ident = "y"}, "foobar"},
	};

	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
		lexer_t *lexer = new_lexer(tests[i].input);
		parser_t *parser = new_parser(lexer);

		program_t *program = parse_program(parser);
		check_parser_errors(parser);

		assertf(program->statements->count == 1, 
		   "Program does not contain 1 statement. got=%d", 
		   program->statements->count);

		statement_t *statement = program->statements->data[0];
		assertf(statement->type == LET_STATEMENT,
		   "wrong statement type. expected LET_STATEMENT, got=%d",
		   statement->type);

		assertf(statement->value->type == tests[i].exp_type,
		   "wrong expression type. expected=%d, got=%d",
		   tests[i].exp_type, statement->value->type);

		switch(tests[i].exp_type) {
			case INTEGER_LITERAL:
				assertf(tests[i].value.integer == statement->value->integer,
				   "wrong integer, expected %d, got %d",
				   tests[i].value.integer, statement->value->integer);
				break;
			case BOOLEAN_EXPR:  
				assertf(tests[i].value.boolean == statement->value->boolean,
				   "wrong boolean, expected %d, got %d",
				   tests[i].value.boolean, statement->value->boolean);
				break;
			case IDENT_EXPR:
				assertf(strcmp(statement->value->ident.value, tests[i].value.ident) == 0,
				   "wrong identifier, expected %s, got %s",
				   tests[i].value.ident, statement->value->ident.value);
				break;
			default:
				break;
		}

		assertf(strcmp(statement->name.value, tests[i].name) == 0,
		   "wrong name value. expected %s, got %s",
		   tests[i].name, statement->name.value);
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

	assertf(program->statements->count == 3, "Program does not container 3 statements");
	
	struct {
		char *literal;
		char *name;
		} tests[] = {
		{"return", "5" },
		{"return", "10"},
		{"return", "993322"},
	};
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		statement_t statement = *(statement_t *)program->statements->data[i];
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
	assertf(program->statements->count == 1, "Program does not container 1 statements");

	statement_t statement = *(statement_t *)program->statements->data[0];

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
	assertf(program->statements->count == 1, "Program does not container 1 statement");
	statement_t statement = *(statement_t *)program->statements->data[0];

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
		assertf(program->statements->count == 1, "Program does not container 1 statement");
		statement_t statement = *(statement_t *)program->statements->data[0];

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

		assertf(program->statements->count == 1, 
		       "program.Statements does not contain 1 statement. got=%d\n",
		       program->statements->count);

		statement_t *stmt = program->statements->data[0];
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
		{"a + add(b * c) + d;", "((a + add((b * c))) + d)"},
		{"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8));", 
		 "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
		{"add(a + b + c * d / f + g);", 
		 "add((((a + b) + ((c * d) / f)) + g))"},
		{"a * [1, 2, 3, 4][b * c] * d",
		 "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
		{"add(a * b[2], b[1], 2 * [1, 2][1])",
		 "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"},
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

		assertf(program->statements->count == 1, 
		       "program has not enough statements. got=%d",
		       program->statements->count);

		statement_t *stmt = program->statements->data[0];
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

	assertf(program->statements->count == 1, 
	"program does not contain 1 statement. got=%d\n",
	program->statements->count);

	statement_t *stmt = program->statements->data[0];
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

	assertf(program->statements->count == 1,
		"program does not contain 1 statement. got=%d\n",
		program->statements->count);

	statement_t *stmt = program->statements->data[0];
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

void test_function_literal_parsing(void) {
	char *input = "fn(x, y) { x + y; }";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);
	program_t *program = parse_program(parser);

	check_parser_errors(parser);

   	assertf(program->statements->count == 1, 
   			"program does not contain 1 statement. got=%d\n",
   			program->statements->count);

   	statement_t *stmt = program->statements->data[0];
   	assertf(stmt->type == EXPRESSION_STATEMENT,
   			"program.statements[0] is not expression statement. got=%d",
   			stmt->type);

   	expression_t *expression = stmt->value;
   	assertf(expression->type == FUNCTION_LITERAL,
   			"statement.expression is not function literal. got=%d",
   			expression->type);

   	function_literal_t *function = &expression->function_literal;
   	assertf(function->parameters->count == 2,
   			"function literal parameters wrong. want 2, got=%d\n",
   			function->parameters->count);

   	// Test function parameters  
   	identifier_t *param1 = function->parameters->data[0];
   	identifier_t *param2 = function->parameters->data[1];
	assertf(strcmp(param1->value, "x") == 0, "expected %s, got %s", "x", param1->value);
	assertf(strcmp(param2->value, "y") == 0, "expected %s, got %s", "y", param1->value);

   	// Test function body
   	block_statement_t *body = function->body;
   	assertf(body->statements->count == 1,
   			"function body has wrong number of statements. got=%d\n",
   			body->statements->count);

   	statement_t *body_stmt = body->statements->data[0];
   	assertf(body_stmt->type == EXPRESSION_STATEMENT, 
   			"function body stmt is not expression statement. got=%d",
   			body_stmt->type);

   	expression_t *body_expr = body_stmt->value;
   	assertf(body_expr->type == INFIX_EXPR,
   			"body statement value is not infix expression. got=%d",
   			body_expr->type);

   	check_identifier(body_expr->infix_expression.left, "x");
   	assertf(strcmp(body_expr->infix_expression.op, "+") == 0,
   			"wrong operator. expected=+, got=%s", 
   			body_expr->infix_expression.op);
   	check_identifier(body_expr->infix_expression.right, "y");
}

void test_call_expression_parsing(void) {
       char *input = "add(1, 2 * 3, 4 + 5);";
       lexer_t *lexer = new_lexer(input);
       parser_t *parser = new_parser(lexer);
       program_t *program = parse_program(parser);

       check_parser_errors(parser);

       assertf(program->statements->count == 1,
               "program does not contain 1 statement. got=%d\n",
               program->statements->count);

       statement_t *stmt = program->statements->data[0];
       assertf(stmt->type == EXPRESSION_STATEMENT,
               "statement is not expression statement. got=%d",
               stmt->type);

       expression_t *exp = stmt->value;
       assertf(exp->type == CALL_EXPRESSION,
               "expression is not call expression. got=%d", 
               exp->type);

       check_identifier(exp->call_expression.function, "add");

       assertf(exp->call_expression.arguments->count == 3,
               "wrong length of arguments. got=%d",
               exp->call_expression.arguments->count);

       expression_t *arg0 = exp->call_expression.arguments->data[0];
       check_integer_literal(arg0, 1);

       expression_t *arg1 = exp->call_expression.arguments->data[1];
       assertf(arg1->type == INFIX_EXPR, "arg1 not infix expression");
       check_integer_literal(arg1->infix_expression.left, 2);
       assertf(strcmp(arg1->infix_expression.op, "*") == 0, "wrong operator");
       check_integer_literal(arg1->infix_expression.right, 3);

       expression_t *arg2 = exp->call_expression.arguments->data[2];
       assertf(arg2->type == INFIX_EXPR, "arg2 not infix expression");
       check_integer_literal(arg2->infix_expression.left, 4);
       assertf(strcmp(arg2->infix_expression.op, "+") == 0, "wrong operator");
       check_integer_literal(arg2->infix_expression.right, 5);
}

void test_string_literal_expression(void) {
	char *input = "\"hello world\";";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);
	program_t *program = parse_program(parser);

	check_parser_errors(parser);
	assertf(program->statements->count == 1,
	    "program does not contain 1 statement. got=%d\n",
	    program->statements->count);

	statement_t *stmt = program->statements->data[0];
	assertf(stmt->type == EXPRESSION_STATEMENT,
	    "program.statements[0] is not expression statement. got=%d",
	    stmt->type);

	expression_t *expression = stmt->value;
	assertf(expression->type == STRING_LITERAL,
	    "expression is not string literal. got=%d",
	    expression->type);

	char *string_literal = string_get_data(expression->string_literal);
	assertf(strcmp(string_literal, "hello world") == 0,
	    "literal.value not %s. got=%s",
	    "hello world", string_literal);
}

void test_array_literal_expression() {
	char *input = "[1, 2 * 3, 4 + 5];";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);
	program_t *program = parse_program(parser);

	check_parser_errors(parser);

	assertf(program->statements->count == 1,
		    "program does not contain 1 statement. got=%d\n",
		    program->statements->count);

	statement_t *stmt = program->statements->data[0];
	assertf(stmt->type == EXPRESSION_STATEMENT,
		    "statement is not expression statement. got=%d",
		    stmt->type);

	expression_t *exp = stmt->value;
	assertf(exp->type == ARRAY_LITERAL,
		    "expression is not array literal. got=%d",
		    exp->type);

	array_literal_t *array = &exp->array_literal;
	assertf(array->elements->count == 3,
		    "array has wrong number of elements. got=%d",
		    array->elements->count);
}

void test_index_expression(void) {
	char *input = "myArray[1 + 1]";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);
	program_t *program = parse_program(parser);

	check_parser_errors(parser);

	assertf(program->statements->count == 1,
		    "program does not contain 1 statement. got=%d\n",
		    program->statements->count);

	statement_t *stmt = program->statements->data[0];
	assertf(stmt->type == EXPRESSION_STATEMENT,
		    "statement is not expression statement. got=%d",
		    stmt->type);

	expression_t *exp = stmt->value;
	assertf(exp->type == INDEX_EXPR,
		    "expression is not index expression. got=%d",
		    exp->type);

	index_expression_t *index_exp = &exp->index_expression;
	check_identifier(index_exp->left, "myArray");
	assertf(index_exp->index->type == INFIX_EXPR,
	"index is not infix expression. got=%d",
            index_exp->index->type);
    
    expression_t *infix = index_exp->index;
    check_integer_literal(infix->infix_expression.left, 1);
    assertf(strcmp(infix->infix_expression.op, "+") == 0,
            "operator is not '+'. got=%s",
            infix->infix_expression.op);
    check_integer_literal(infix->infix_expression.right, 1);
}

void check_infix_expression(expression_t *exp, int left_value, char *operator, int right_value) {
    // First verify it's an infix expression
    assertf(exp->type == INFIX_EXPR,
            "expression is not infix expression. got=%d",
            exp->type);
    
    // Check operator
    assertf(strcmp(exp->infix_expression.op, operator) == 0,
            "operator is not '%s'. got=%s",
            operator, exp->infix_expression.op);
    
    // Check left operand is integer literal with correct value
    expression_t *left = exp->infix_expression.left;
    assertf(left->type == INTEGER_LITERAL,
            "left expression is not integer literal. got=%d",
            left->type);
    assertf(left->integer == left_value,
            "left value not %d. got=%d",
            left_value, left->integer);
    
    // Check right operand is integer literal with correct value  
    expression_t *right = exp->infix_expression.right;
    assertf(right->type == INTEGER_LITERAL,
            "right expression is not integer literal. got=%d",
            right->type);
    assertf(right->integer == right_value,
            "right value not %d. got=%d", 
            right_value, right->integer);
}

void test_hash_literal_string_keys() {
    char *input = "{\"one\": 1, \"two\": 2, \"three\": 3}";
    lexer_t *lexer = new_lexer(input);
    parser_t *parser = new_parser(lexer);
    program_t *program = parse_program(parser);

    check_parser_errors(parser);

    assertf(program->statements->count == 1,
            "program does not contain 1 statement. got=%d\n",
            program->statements->count);

    statement_t *stmt = program->statements->data[0];
    assertf(stmt->type == EXPRESSION_STATEMENT,
            "statement is not expression statement. got=%d",
            stmt->type);

    expression_t *exp = stmt->value;
    assertf(exp->type == HASH_LITERAL,
            "expression is not hash literal. got=%d",
            exp->type);

    // Check length
    assertf(exp->hash_literal.pairs_len == 3,
            "hash has wrong length. got=%d",
            exp->hash_literal.pairs_len);

    // Create test key expressions
    token_t one_token = {.type = STRING, .literal = "one"};
    expression_t *one_key = new_expression(STRING_LITERAL, one_token);
    one_key->string_literal = string_from("one");

    token_t two_token = {.type = STRING, .literal = "two"};
    expression_t *two_key = new_expression(STRING_LITERAL, two_token);
    two_key->string_literal = string_from("two");

    token_t three_token = {.type = STRING, .literal = "three"};
    expression_t *three_key = new_expression(STRING_LITERAL, three_token);
    three_key->string_literal = string_from("three");

    // Test expected values
    expression_t *value;
    value = parser_hash_get_value(&exp->hash_literal, one_key);
    assertf(value != NULL, "value for key 'one' not found");
    check_integer_literal(value, 1);

    value = parser_hash_get_value(&exp->hash_literal, two_key);
    assertf(value != NULL, "value for key 'two' not found");
    check_integer_literal(value, 2);

    value = parser_hash_get_value(&exp->hash_literal, three_key);
    assertf(value != NULL, "value for key 'three' not found");
    check_integer_literal(value, 3);
}

void test_empty_hash_literal() {
    char *input = "{}";
    lexer_t *lexer = new_lexer(input);
    parser_t *parser = new_parser(lexer);
    program_t *program = parse_program(parser);

    check_parser_errors(parser);

    assertf(program->statements->count == 1,
            "program does not contain 1 statement. got=%d\n",
            program->statements->count);

    statement_t *stmt = program->statements->data[0];
    assertf(stmt->type == EXPRESSION_STATEMENT,
            "statement is not expression statement. got=%d",
            stmt->type);

    expression_t *exp = stmt->value;
    assertf(exp->type == HASH_LITERAL,
            "expression is not hash literal. got=%d",
            exp->type);

    assertf(exp->hash_literal.pairs_len == 0,
            "hash.Pairs has wrong length. got=%d",
            exp->hash_literal.pairs_len);
}

void test_hash_literal_with_expressions() {
    char *input = "{\"one\": 0 + 1, \"two\": 10 - 8, \"three\": 15 / 5}";
    lexer_t *lexer = new_lexer(input);
    parser_t *parser = new_parser(lexer);
    program_t *program = parse_program(parser);

    check_parser_errors(parser);

    assertf(program->statements->count == 1,
            "program does not contain 1 statement. got=%d\n",
            program->statements->count);

    statement_t *stmt = program->statements->data[0];
    assertf(stmt->type == EXPRESSION_STATEMENT,
            "statement is not expression statement. got=%d",
            stmt->type);

    expression_t *exp = stmt->value;
    assertf(exp->type == HASH_LITERAL,
            "expression is not hash literal. got=%d",
            exp->type);

    assertf(exp->hash_literal.pairs_len == 3,
            "hash has wrong length. got=%d",
            exp->hash_literal.pairs_len);

    // Create test key expressions
    token_t one_token = {.type = STRING, .literal = "one"};
    expression_t *one_key = new_expression(STRING_LITERAL, one_token);
    one_key->string_literal = string_from("one");

    token_t two_token = {.type = STRING, .literal = "two"};
    expression_t *two_key = new_expression(STRING_LITERAL, two_token);
    two_key->string_literal = string_from("two");

    token_t three_token = {.type = STRING, .literal = "three"};
    expression_t *three_key = new_expression(STRING_LITERAL, three_token);
    three_key->string_literal = string_from("three");

    // Test expressions
    expression_t *value;

    // Test "one": 0 + 1
    value = parser_hash_get_value(&exp->hash_literal, one_key);
    assertf(value != NULL, "value for key 'one' not found");
    check_infix_expression(value, 0, "+", 1);

    // Test "two": 10 - 8  
    value = parser_hash_get_value(&exp->hash_literal, two_key);
    assertf(value != NULL, "value for key 'two' not found");
    check_infix_expression(value, 10, "-", 8);

    // Test "three": 15 / 5
    value = parser_hash_get_value(&exp->hash_literal, three_key);
    assertf(value != NULL, "value for key 'three' not found");
    check_infix_expression(value, 15, "/", 5);
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
	TEST(test_function_literal_parsing);
	TEST(test_call_expression_parsing);
	TEST(test_string_literal_expression);
	TEST(test_array_literal_expression);
	TEST(test_index_expression);
	TEST(test_hash_literal_string_keys);
	TEST(test_empty_hash_literal);
	TEST(test_hash_literal_with_expressions);
}
