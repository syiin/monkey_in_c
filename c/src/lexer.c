#include "lexer.h"
#include <stdlib.h>
#include <string.h>

lexer_t *new_lexer(char *input){
	lexer_t *lexer = malloc(sizeof(lexer_t));
	if (lexer == NULL){
		return NULL;
	}
	lexer->input = strdup(input);
	return lexer;
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
