CC = gcc
CFLAGS+= -Werror -Wall -Isrc/ -g
VPATH= src
TOKEN_SRC= token.c
LEXER_SRC= lexer.c $(TOKEN_SRC)
REPL_SRC = repl.c ${LEXER_SRC}
TESTS= bin/lexer_test

all: bin/monkey
bin/:
	mkdir -p bin/
bin/monkey: main.c $(REPL_SRC) | bin/
	$(CC) $(CFLAGS) $^ -Ofast -march=native -o $@
bin/lexer_test: tests/lexer_test.c $(REPL_SRC) | bin/
	$(CC) $(CFLAGS) $^ -o $@

check: $(TESTS)
	for test in $^; do $$test || exit 1; done
