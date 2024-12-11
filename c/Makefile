CC = gcc
CFLAGS+= -Werror -Wall -Isrc/ -g
VPATH= src
TOKEN_SRC= token.c
LEXER_SRC= lexer.c $(TOKEN_SRC)
REPL_SRC = repl.c ${LEXER_SRC}
AST_SRC = ast.c ${REPL_SRC}
PARSER_SRC = parser.c ${REPL_SRC}
TESTS= bin/lexer_test bin/parser_test

all: bin/monkey
bin/:
	mkdir -p bin/
bin/monkey: main.c $(PARSER_SRC) | bin/
	$(CC) $(CFLAGS) $^ -Ofast -march=native -o $@
bin/lexer_test: tests/lexer_test.c $(LEXER_SRC) | bin/
	$(CC) $(CFLAGS) $^ -o $@
bin/parser_test: tests/parser_test.c $(PARSER_SRC) | bin/
	$(CC) $(CFLAGS) $^ -o $@

check: $(TESTS)
	for test in $^; do $$test || exit 1; done
