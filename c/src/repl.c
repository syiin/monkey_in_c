void start(FILE *in, FILE *out){
	char input[1024] = { '\0' }
	while(true){
		printf(">> ")
		if (fgets(input, sizeof(input), in) == NULL){
			return;
		}

		lexer_t lexer = new_lexer(input);
		token_t *token = next_token(lexer);
		while(token->type != EOF_TOKEN){
			print_token(token);
			token = next_token(lexer);
		}
	}
	
}
