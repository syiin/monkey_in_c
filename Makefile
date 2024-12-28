CC = gcc
CFLAGS+= -Werror -Wall -Isrc/ -g
VPATH= src
VECTOR_SRC = vector.c custom_string.c
TOKEN_SRC= token.c $(VECTOR_SRC)
LEXER_SRC= lexer.c $(TOKEN_SRC)
REPL_SRC = repl.c ${LEXER_SRC}
PARSER_SRC = parser.c ast.c ${REPL_SRC}
EVAL_SRC = object.c ${PARSER_SRC}
TESTS= bin/lexer_test bin/parser_test bin/ast_test bin/evaluator_test

all: bin/monkey
bin/:
	mkdir -p bin/
bin/monkey: main.c $(PARSER_SRC) | bin/
	$(CC) $(CFLAGS) $^ -Ofast -march=native -o $@
bin/lexer_test: tests/lexer_test.c $(LEXER_SRC) | bin/
	$(CC) $(CFLAGS) $^ -o $@
bin/parser_test: tests/parser_test.c $(PARSER_SRC) | bin/
	$(CC) $(CFLAGS) $^ -o $@
bin/ast_test: tests/ast_test.c $(PARSER_SRC) | bin/
	$(CC) $(CFLAGS) $^ -o $@
bin/evaluator_test: tests/evaluator_test.c $(EVAL_SRC) | bin/
	$(CC) $(CFLAGS) $^ -o $@

check: $(TESTS)
	for test in $^; do $$test || exit 1; done
