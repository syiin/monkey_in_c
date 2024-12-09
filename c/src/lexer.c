#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include "token.h"


token_t *next_token(lexer_t *l){
	token_t *tok = new_token(ILLEGAL, "");
	switch (l->ch){
		case '=':
			tok->type = ASSIGN;
			tok->literal = "=";
			break;
		case ';':
			tok->type = SEMICOLON;
			tok->literal = ";";
			break;
		case '(':
			tok->type = LPAREN;
			tok->literal = "(";
			break;
		case ')':
			tok->type = RPAREN;
			tok->literal = ")";
			break;
		case ',':
			tok->type = COMMA;
			tok->literal = ",";
			break;
		case '+':
			tok->type = PLUS;
			tok->literal = "+";
			break;
		case '{':
			tok->type = LBRACE;
			tok->literal = "{";
			break;
		case '}':
			tok->type = RBRACE;
			tok->literal = "}";
			break;
		case 0:
			tok->type = EOF_TOKEN;
			tok->literal = "";
			break;
	}
	read_char(l);
	return tok;
}

void read_char(lexer_t *l){
	//TODO: store length of input once?
	if (l->read_position >= strlen(l->input)){
		l->ch = 0;
	} else {
		l->ch = l->input[l->read_position];
	}
	l->position = l->read_position;
	l->read_position++;
}


lexer_t *new_lexer(char *input){
	lexer_t *lexer = malloc(sizeof(lexer_t));
	if (lexer == NULL){
		return NULL;
	}
	lexer->input = strdup(input);
	read_char(lexer);
	return lexer;
}
