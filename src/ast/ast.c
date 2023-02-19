/*
 *	Made by Noah Van Miert
 *	4/02/2023
*/

#include "ast.h"

#include <stdlib.h>
#include <stdio.h>


/*
 *	Allocates and initializes an new AST object.
 *	@param type: The AST type.
*/
ast_t *ast_new(ast_type type)
{
	ast_t *ast = malloc(sizeof(ast_t));

	if (!ast) { 
		fprintf(stderr, "error: memory allocation failed");
		exit(1); 
	}

	ast->type = type;

	ast->variable_offset = 0;

	/* AST_UINT32 */
	ast->uint32_value = 0;

	/* AST_FUNCTION_DEF */
	ast->function_def_name = NULL;
	ast->function_def_body = NULL;

	/* AST_FUNCTION_CALL */
	ast->function_call_name = NULL;
	
	/* AST_VARIABLE_DEF */
	ast->variable_def_name = NULL;
	ast->variable_def_value = NULL;
	ast->variable_def_type = TYPE_UNKOWN;

	/* AST_VARIABLE */
	ast->variable_name = NULL;

	/* AST_VARIABLE_REDEF */
	ast->variable_redef_name = NULL;
	ast->variable_def_value = NULL;

	/* AST_COMPOUND */
	ast->compound_value = NULL;
	ast->compound_size = 0;

	return ast;
}
