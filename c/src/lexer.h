#ifndef LEXER_H
#define LEXER_H

typedef struct Lexer {
	char*	input;
	int	position;
	int	read_position;
	char    ch;
} lexer_t;

lexer_t *new_lexer(char *input);

#endif
