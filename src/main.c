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
	scope_t globl_scope;
	
	lexer_init(&lexer, "examples/write.apo");
	parser_init(&parser, &lexer);
	scope_init(&globl_scope);

	parser_parse(&parser, &globl_scope);

    return 0;
}
