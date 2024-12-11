#include "test_helpers.h"
#include "../src/lexer.h"
#include "../src/parser.h"

void test_let_statements(){
	char *input = "let x = 5;\n"
		"let y = 10;\n"
		"let foobar = 838383;\n";
	lexer_t *lexer = new_lexer(input);
	parser_t *parser = new_parser(lexer);

	program_t *program = parse_program(parser);
	if (program == NULL){
		printf("Parse program failed \n");
		exit(1);
	}

	assertf(program->size == 3, "Program does not container 3 statements");

	struct { char *name; } tests[] = {
		{"x"},
		{"y"},
		{"foorbar"},
	};
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		statement_t statement = program->statements[i];
		assertf(strcmp(statement.token.literal, tests[i].name) == 0, "wrong literal. expected %s, got %s\n", tests[i].name, statement.token.literal);
		assertf(strcmp(statement.name.value, tests[i].name) == 0, "wrong name value. expected %s, got %s\n", tests[i].name, statement.name.value);
		assertf(strcmp(statement.name.token.literal, tests[i].name) == 0, "wrong name literal. expected %s, got %s", tests[i].name, statement.token.literal);
	}
}

int main(int argc, char *argv[]) {
	TEST(test_let_statements);
}
