#include "../src/token.h"
#include "../src/lexer.h"
#include "test_helpers.h"

void test_lexer(){
	char *input = "=+(){},;";
	token_t tests[] = {
		{ASSIGN, "="},
		{PLUS, "+"},
		{LPAREN, "("},
		{RPAREN, ")"},
		{LBRACE, "{"},
		{RBRACE, "}"},
		{COMMA, ","},
		{SEMICOLON, ";"},
		{EOF_TOKEN, ""},
	};

	lexer_t *l = new_lexer(input);
	token_t t;
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		next_token(l, &t);
		assertf(t.type == tests[i].type, "[%d] wrong type: expected \"%s\", got \"%s\"\n", i, token_type_to_string(tests[i].type), token_type_to_string(t.type));
	}

}
