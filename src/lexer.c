#include "lexer.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

token_t *lexer_next_token(lexer_t *lexer){
	token_t *tok;
	skip_whitespace(lexer);

	switch (lexer->ch){
		case '=':
			if (peek_char(lexer) == '='){
				read_char(lexer);
				tok = new_token(EQ, "==");

			} else {
				tok = new_token(ASSIGN, "=");
			}
			break;
		case ';':
			tok = new_token(SEMICOLON, ";");
			break;
		case '(':
			tok = new_token(LPAREN, "(");
			break;
		case ')':
			tok = new_token(RPAREN, ")");
			break;
		case ',':
			tok = new_token(COMMA, ",");
			break;
		case '+':
			tok = new_token(PLUS, "+");
			break;
		case '-':
			tok = new_token(MINUS, "-");
			break;
		case '!':
			if (peek_char(lexer) == '='){
				read_char(lexer);
				tok = new_token(NOT_EQ, "!=");
			} else {
				tok = new_token(BANG, "!");
			}
			break;
		case '/':
			tok = new_token(SLASH, "/");
			break;
		case '*':
			tok = new_token(ASTERISK, "*");
			break;
		case '<':
			tok = new_token(LT, "<");
			break;
		case '>':
			tok = new_token(GT, ">");
			break;
		case '{':
			tok = new_token(LBRACE, "{");
			break;
		case '}':
			tok = new_token(RBRACE, "}");
			break;
		case '"':
			tok = new_token(STRING, read_string(lexer));
			break;
		case 0:
			tok = new_token(EOF_TOKEN, "");
			break;
		default:
			if (is_letter(lexer->ch)){
				char *literal = read_identifier(lexer);
				return new_token(lookup_ident(literal), literal);
			} else if (is_digit(lexer->ch)){
				char *literal = read_number(lexer);
				return new_token(INT, literal);
			} else {
				char str[2];
				str[0] = lexer->ch;
				str[1] = '\0';
				tok = new_token(ILLEGAL, strdup(str));
			}
	}
	read_char(lexer);
	return tok;
}

char *read_string(lexer_t *l){
	int position = l->position+1;
	read_char(l);
	while(l->ch != '"' && l->ch != 0){
		read_char(l);
	}
	size_t length = l->position - position;
	char *result_string = malloc(length + 1);
	if (result_string == NULL) {
		return NULL;
	}

	strncpy(result_string, l->input + position, length);
	result_string[length] = '\0';
	read_char(l);
	return result_string;
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

TokenType lookup_ident(char *literal){
	if (strcmp(literal, "fn") == 0){
		return FUNCTION;
	} else if (strcmp(literal, "let") == 0){
		return LET;
	} else if (strcmp(literal, "true") == 0){
		return TRUE;
	} else if (strcmp(literal, "false") == 0){
		return FALSE;
	} else if (strcmp(literal, "if") == 0){
		return IF;
	} else if (strcmp(literal, "else") == 0){
		return ELSE;
	} else if (strcmp(literal, "return") == 0){
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
	lexer->read_position = 0;
	read_char(lexer);
	return lexer;
}
