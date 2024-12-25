#include <stdio.h>
#include "repl.h"

int main(){
	printf("Hello! Welcome to C Monkeys!\n");
	repl_start(stdin, stdout);
}
