/*
 *	Made by Noah Van Miert
 *	27/01/2023
*/


#include <stdio.h>
#include "lexer/lexer.h"


int main(void)
{
	lexer_t lexer;

	lexer_init(&lexer, "examples/write.apo");

	token_t *tok;

	while ((tok = lexer_get_token(&lexer))) {
		printf("%s:%zu:%zu: '%s' type: '%s'\n", tok->filepath, tok->line, tok->col, tok->value, get_token_str(tok->type));
	}

    return 0;
}
