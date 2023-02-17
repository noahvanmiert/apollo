/*
 *	Made by Noah Van Miert
 *	4/02/2023
*/

#include "ast.h"

#include <stdlib.h>
#include <stdio.h>


ast_t *ast_new(ast_type type)
{
	ast_t *ast = malloc(sizeof(ast_t));

	if (!ast) { 
		fprintf(stderr, "error: memory allocation failed");
		exit(1); 
	}

	ast->type = type;

	ast->variable_offset = 0;

	ast->uint32_value = 0;

	ast->function_def_name = NULL;
	ast->function_def_body = NULL;
	
	ast->function_call_name = NULL;

	ast->variable_def_name = NULL;
	ast->variable_def_value = NULL;

	ast->compound_value = NULL;
	ast->compound_size = 0;

	return ast;
}
