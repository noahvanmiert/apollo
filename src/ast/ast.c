/*
 *	Made by Noah Van Miert
 *	4/02/2023
*/

#include "ast.h"

#include <stdlib.h>

struct Ast *create_ast(enum AstType type)
{
	struct Ast *ast = malloc(sizeof(struct Ast));

	ast->type = type;

	ast->function_def_name = NULL;
	ast->function_def_body = NULL;
	
	ast->function_call_name = NULL;

	ast->compound_value = NULL;
	ast->compound_size = 0;

	return ast;
}
