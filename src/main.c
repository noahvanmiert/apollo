/*
 *	Made by Noah Van Miert
 *	27/01/2023
*/


#include <stdio.h>
#include "lexer/lexer.h"


int main(void)
{
	struct Lexer lexer;

	lexer.data = "fn main() { print(1); }";
	lexer.index = 0;
	lexer.current = lexer.data[0];

	struct Token *tok;

	while ((tok = lexer_get_token(&lexer))) {
		printf("token: '%s' type: '%s'\n", tok->value, get_token_str(tok->type));
	}

    return 0;
}
