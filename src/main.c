/*
 *	Made by Noah Van Miert
 *	27/01/2023
*/

#include <stdio.h>
#include "parser/parser.h"
#include "compiler/compiler.h"


int main(void)
{
	lexer_t lexer;
	parser_t parser;
	scope_t globl_scope;
	
	lexer_init(&lexer, "examples/write.apo");
	parser_init(&parser, &lexer);
	scope_init(&globl_scope);

	ast_t *root = parser_parse(&parser, &globl_scope);
	
	compiler_compile(root);
	compiler_write_asm("output.s");

    return 0;
}
