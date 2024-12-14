#include "test_helpers.h"
#include "../src/ast.h"
#include "../src/token.h"
#include "../src/parser.h"

void test_ast_string(){
	int initial_cap = 2;
	program_t *program = malloc(sizeof(program_t) + sizeof(statement_t) * initial_cap);
	program->statement_cap = initial_cap;
	program->count = 0;

	statement_t *statement = &(statement_t){
		.type = LET_STATEMENT,
		.token = (token_t){
			.type = LET,
			.literal = "let",
		},
		.name = (identifier_t){
			.token = (token_t){
				.type = IDENT,
				.literal = "myVar",
			},
			.value = "myVar"
		},
		.value = &(expression_t){
			.token = (token_t){
				.type = IDENT,
				.literal = "anotherVar"
			},
			.ident = ( identifier_t ){
				.token = (token_t){
					.type = IDENT,
					.literal = "anotherVar",
				},
				.value = "anotherVar",
			},
		},
	};
	push_to_program(statement, program);

	char program_str[256];
	ast_string(program_str, program);
	char *expected_str = "let myVar = anotherVar;";
	
	assertf(strcmp(expected_str, program_str) == 0, "wrong ast string. expected %s, got %s\n", program_str, expected_str);
}

int main(int argc, char *argv[]) {
	TEST(test_ast_string);
}
