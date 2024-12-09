#include <stdio.h>
#include "token.h"

int main(){
	/*token_t *token = new_token("IDENT", "x");*/
	token_t *token = new_token(IDENT, "x");
	print_token(token);

}
