#include "lexer.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>


token_t *lexer_next_token(lexer_t *l){
	token_t *tok = new_token(ILLEGAL, "");
	
	skip_whitespace(l);

	switch (l->ch){
		case '=':
			if (peek_char(l) == '='){
				read_char(l);
				tok->type = EQ;
				tok->literal = "==";

			} else {
				tok->type = ASSIGN;
				tok->literal = "=";
			}
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
		case '-':
			tok->type = MINUS;
			tok->literal = "-";
			break;
		case '!':
			if (peek_char(l) == '='){
				read_char(l);
				tok->type = NOT_EQ;
				tok->literal = "!=";
			} else {
				tok->type = BANG;
				tok->literal = "!";
			}
			break;
		case '/':
			tok->type = SLASH;
			tok->literal = "/";
			break;
		case '*':
			tok->type = ASTERISK;
			tok->literal = "*";
			break;
		case '<':
			tok->type = LT;
			tok->literal = "<";
			break;
		case '>':
			tok->type = GT;
			tok->literal = ">";
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
		default:
			if (is_letter(l->ch)){
				tok->literal = read_identifier(l);
				tok->type = lookup_ident(tok);
				return tok;
			} else if (is_digit(l->ch)){
				tok->type = INT;
				tok->literal = read_number(l);
				return tok;
			} else {
				tok->type = ILLEGAL;
				char str[2];
				str[0] = l->ch;
				str[1] = '\0';
				tok->literal = str;
			}
	}
	read_char(l);
	return tok;
}

char *read_identifier(lexer_t *l){
	int position = l->position;
	while(is_letter(l->ch)){
		read_char(l);
	}
	size_t length = l->position - position;
	char *result_string = malloc(length + 1);
	if (result_string == NULL) {
		return NULL;
	}

	strncpy(result_string, l->input + position, length);
	result_string[length] = '\0';
	return result_string;
}

char *read_number(lexer_t *l){
	int position = l->position;
	while(is_digit(l->ch)){
		read_char(l);
	}
	size_t length = l->position - position;
	char *result_string = malloc(length + 1);
	if (result_string == NULL) {
		return NULL;
	}

	strncpy(result_string, l->input + position, length);
	result_string[length] = '\0';
	return result_string;
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

char peek_char(lexer_t *l){
	if (l->read_position >= strlen(l->input)){
		return 0;
	} else {
		return l->input[l->read_position];
	}
}

void skip_whitespace(lexer_t *l){
	while (l->ch == ' ' || l->ch == '\t' || l->ch =='\n' || l->ch == '\r'){
		read_char(l);
	}
}

TokenType lookup_ident(token_t *token){
	if (strcmp(token->literal, "fn") == 0){
		return FUNCTION;
	} else if (strcmp(token->literal, "let") == 0){
		return LET;
	} else if (strcmp(token->literal, "true") == 0){
		return TRUE;
	} else if (strcmp(token->literal, "false") == 0){
		return FALSE;
	} else if (strcmp(token->literal, "if") == 0){
		return IF;
	} else if (strcmp(token->literal, "else") == 0){
		return ELSE;
	} else if (strcmp(token->literal, "return") == 0){
		return RETURN;
	} else {
		return IDENT;
	}
}


bool is_letter(unsigned char ch) {
    return ((ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch == '_'));
}

bool is_digit(unsigned char ch) {
	return (('0' <= ch) && (ch <= '9'));
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
