#include "test_helpers.h"
#include "../src/lexer.h"
#include "../src/parser.h"

void check_parser_errors(parser_t *parser){
	if (parser->errors->count > 0){
		print_errors(parser);
	}
	assertf(parser->errors->count == 0, "expected no errors but got %d", 0, parser->errors->count);
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
		/*assertf(strcmp(statement.name.token.literal, tests[i].name) == 0, "wrong name literal. expected %s, got %s", tests[i].value, statement.token.literal);*/
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
	assertf(statement.value->integer == 5, "wrong value. expected %d, got %d\n", 5, statement.value->integer);
	assertf(strcmp(statement.value->token.literal, "5") == 0, "wrong literal. expected %s, got %s\n", "foobar", statement.token.literal);
}

void test_parsing_prefix_expressions(){
	struct {
		char	*input;
		char	*operator;
		int	integer_value;
	} tests[] = {
		{"!5;", "!", 5},
		{"-15;", "-", 15},
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
	}
}

int main(int argc, char *argv[]) {
	TEST(test_let_statements);
	TEST(test_return_statements);
	TEST(test_identifier);
	TEST(test_integer);
	TEST(test_parsing_prefix_expressions);
}
