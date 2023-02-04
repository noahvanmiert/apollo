/*
 *	Made by Noah Van Miert
 *	4/02/2023
*/

#ifndef __AST_H_
#define __AST_H_

#include <stddef.h>


enum AstType {
	AST_COMPOUND,
	AST_FUNCTION_DEF,
	AST_FUNCTION_CALL
};


struct Ast {
	enum AstType type;

	const char *function_def_name;
	struct Ast *function_def_body;
	
	const char *function_call_name;

	struct Ast **compound_value;
	size_t compound_size;
};


struct Ast *create_ast(enum AstType type);


#endif // __AST_H_
