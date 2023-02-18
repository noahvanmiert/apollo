/*
 *	Made by Noah Van Miert
 *	4/02/2023
*/

#ifndef __AST_H_
#define __AST_H_

#include <stddef.h>
#include <stdint.h>

#include "../parser/stackframe/stackframe.h"


typedef enum {
	AST_COMPOUND,
	AST_UINT32,
	AST_FUNCTION_DEF,
	AST_FUNCTION_CALL,
	AST_VARIABLE_DEF,
	AST_NOP
} ast_type;


typedef struct ast_t {
	ast_type type;

	/* Only needed on arm64 */
	stack_frame_t stackframe;

	/* The offset from rbp or sp */
	size_t variable_offset;

	/* AST_UINT32 */
	uint32_t uint32_value;

	/* AST_FUNCTION_DEF */
	const char *function_def_name;
	struct ast_t *function_def_body;
	
	/* AST_FUNCTION_CALL */
	const char *function_call_name;

	/* AST_VARIABLE_DEF */
	const char *variable_def_name;
	struct ast_t *variable_def_value;

	/* AST_COMPOUND */
	struct ast_t **compound_value;
	size_t compound_size;
} ast_t;


ast_t *ast_new(ast_type type);


#endif // __AST_H_
