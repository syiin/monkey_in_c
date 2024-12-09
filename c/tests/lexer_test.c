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
	token_t *t;
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		t = next_token(l);
		assertf(t->type == tests[i].type, "[%d] wrong type: expected \"%s\", got \"%s\"\n", i, token_type_to_string(tests[i].type), token_type_to_string(t->type));
	}

}

void test_next_token(){
	char *input = "let five = 5;\n"
		"let ten = 10;\n"
		"let add = fn(x, y) {\n"
		"\tx + y;\n"
		"};\n"
		"let result = add(five, ten);\n"
		"!-/*5;\n"
		"5 < 10 > 5;\n";
	
	token_t tests[] = {
		{LET, "let"},
		{IDENT, "five"},
		{ASSIGN, "="},
		{INT, "5"},
		{SEMICOLON, ";"},
		{LET, "let"},
		{IDENT, "ten"},
		{ASSIGN, "="},
		{INT, "10"},
		{SEMICOLON, ";"},
		{LET, "let"},
		{IDENT, "add"},
		{ASSIGN, "="},
		{FUNCTION, "fn"},
		{LPAREN, "("},
		{IDENT, "x"},
		{COMMA, ","},
		{IDENT, "y"},
		{RPAREN, ")"},
		{LBRACE, "{"},
		{IDENT, "x"},
		{PLUS, "+"},
		{IDENT, "y"},
		{SEMICOLON, ";"},
		{RBRACE, "}"},
		{SEMICOLON, ";"},
		{LET, "let"},
		{IDENT, "result"},
		{ASSIGN, "="},
		{IDENT, "add"},
		{LPAREN, "("},
		{IDENT, "five"},
		{COMMA, ","},
		{IDENT, "ten"},
		{RPAREN, ")"},
		{SEMICOLON, ";"},
		{BANG, "!"},
		{MINUS, "-"},
		{SLASH, "/"},
		{ASTERISK, "*"},
		{INT, "5"},
		{SEMICOLON, ";"},
		{INT, "5"},
		{LT, "<"},
		{INT, "10"},
		{GT, ">"},
		{INT, "5"},
		{SEMICOLON, ";"},
		{EOF_TOKEN, ""}	
	};
	lexer_t *l = new_lexer(input);
	token_t *t;
	for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++){
		t = next_token(l);
		assertf(t->type == tests[i].type, "[%d] wrong type: expected \"%s\", got \"%s\"\n", i, token_type_to_string(tests[i].type), token_type_to_string(t->type));
	}
}


int main(int argc, char *argv[]) {
	TEST(test_lexer);
	TEST(test_next_token);
}
