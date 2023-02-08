/*
 *	Made by Noah Van Miert
 *	4/02/2023
*/

#ifndef __AST_H_
#define __AST_H_

#include <stddef.h>


typedef enum {
	AST_COMPOUND,
	AST_FUNCTION_DEF,
	AST_FUNCTION_CALL
} ast_type;


typedef struct ast_t {
	ast_type type;

	/* AST_FUNCTION_DEF */
	const char *function_def_name;
	struct ast_t *function_def_body;
	
	/* AST_FUNCTION_CALL */
	const char *function_call_name;

	/* AST_COMPOUND */
	struct ast_t **compound_value;
	size_t compound_size;
} ast_t;


ast_t *ast_new(ast_type type);


#endif // __AST_H_
