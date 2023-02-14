/*
 *	Made by Noah Van Miert
 *	27/01/2023
*/

#include <stdio.h>
#include "parser/parser.h"
#include "compiler/compiler.h"
#include "apollo/apollo.h"


flag_info_t flag_info;


int main(int argc, char **argv)
{
	lexer_t lexer;
	parser_t parser;
	scope_t globl_scope;

	flag_info = apollo_parse_arguments(argc, argv);
	
	lexer_init(&lexer, flag_info.input_filepath);
	parser_init(&parser, &lexer);
	scope_init(&globl_scope);

	ast_t *root = parser_parse(&parser, &globl_scope);
	
	compiler_compile(root);
	compiler_write_asm("output.s");

	if (!flag_info.only_asm)
		apollo_assemble();

	/* prints 'compilated completed succesfully' in green */
	printf("\033[32;1mCompilation completed succesfully\033[0m\n");

    return 0;
}
