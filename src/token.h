#ifndef TOKEN_H
#define TOKEN_H


typedef enum {
	ILLEGAL,
	EOF_TOKEN,
	// Identifiers + literals
	IDENT, // add, foobar, x, y, ...
	INT, // 1343456
	// Operators
	ASSIGN,
	PLUS,
	MINUS,
	BANG,
	ASTERISK,
	SLASH,
	EQ,
	NOT_EQ,
	LT,
	GT,
	// Delimiters
	COMMA,
	SEMICOLON,
	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE,
	// Keywords
	FUNCTION,
	LET,
	TRUE,
	FALSE,
	IF,
	ELSE,
	RETURN,

	TOKEN_TYPE_COUNT,
} TokenType;

static const char *TOKEN_TYPE_STRINGS[TOKEN_TYPE_COUNT] = {
	[ILLEGAL]   = "ILLEGAL",
	[EOF_TOKEN] = "EOF_TOKEN",
	[IDENT]     = "IDENT",
	[INT]       = "INT",
	[ASSIGN]    = "ASSIGN",
	[PLUS]     = "PLUS",
	[MINUS] = "-",
	[BANG] = "!",
	[ASTERISK] = "*",
	[SLASH] = "/",
	[EQ] = "==",
	[NOT_EQ] = "!=",
	[LT] = "<",
	[GT] = ">",
	[COMMA]     = "COMMA",
	[SEMICOLON] = "SEMICOLON",
	[LPAREN]    = "LPAREN",
	[RPAREN]    = "RPAREN",
	[LBRACE]    = "LBRACE",
	[RBRACE]    = "RBRACE",
	[FUNCTION]  = "FUNCTION",
	[LET]       = "LET",
	[FALSE] = "FALSE",
	[TRUE] = "TRUE"
};

typedef struct Token {
	TokenType	type;
	char		*literal;
} token_t;

const char *token_type_to_string(TokenType t);
TokenType string_to_token_type(const char *str);
token_t *new_token(TokenType type, char *literal);
void free_token(token_t *token);
void print_token(token_t *token);
void copy_token(token_t *dest, token_t *src);

#endif
