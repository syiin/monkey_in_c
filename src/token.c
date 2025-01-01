#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "token.h"

const char *token_type_to_string(TokenType t) {
    if (t < 0 || t >= TOKEN_TYPE_COUNT) {
        return "UNKNOWN";
    }
    return TOKEN_TYPE_STRINGS[t];
}

TokenType string_to_token_type(const char *str) {
    for (int i = 0; i < TOKEN_TYPE_COUNT; i++) {
        if (strcmp(TOKEN_TYPE_STRINGS[i], str) == 0) {
            return (TokenType)i;
        }
    }
    return TOKEN_TYPE_COUNT; // invalid indicator
}


token_t *new_token(TokenType type, char *literal){
	token_t *token = malloc(sizeof(token_t));
	token->type = type;
	token->literal = strdup(literal);
	return token;
}

void free_token(token_t *token){
	free(token->literal);
	free(token);
}

void print_token(token_t *token){
	printf("type: %s, literal: %s \n", token_type_to_string(token->type), token->literal);
}

void copy_token(token_t *dest, token_t *src) {
    dest->type = src->type;
    if (src->literal != NULL) {
        dest->literal = strdup(src->literal);
    } else {
	dest->literal = NULL;
    }
}
