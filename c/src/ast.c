#include "token.h"
#include "ast.h"

void token_literal(program_t *program){
	for (int i = 0; i < program->size; i++){
		print_token(&program->statements[i].token);
	};
}
