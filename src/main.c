/*
 *	Made by Noah Van Miert
 *	27/01/2023
*/

#include <stdio.h>
#include "parser/parser.h"


int main(void)
{
	lexer_t lexer;
	parser_t parser;

	lexer_init(&lexer, "examples/write.apo");
	parser_init(&parser, &lexer);

	printf("%s\n", parser.current->value);

    return 0;
}
