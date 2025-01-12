#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "token.h"

typedef struct Lexer {
	char	*input;
	int	position;
	int	read_position;
	char    ch;
} lexer_t;

lexer_t *new_lexer(char *input);
void read_char(lexer_t *l);
char peek_char(lexer_t *l);
void skip_whitespace(lexer_t *l);
token_t *lexer_next_token(lexer_t *l);
char *read_identifier(lexer_t *l);
char *read_number(lexer_t *l);
bool is_letter(unsigned char ch);
bool is_digit(unsigned char ch);
TokenType lookup_ident(char *literal);
char *read_string(lexer_t *l);

#endif
