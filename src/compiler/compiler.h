/*
 *	Made by Noah Van Miert
 *	11/02/2023
*/

#ifndef __COMPILER_H_
#define __COMPILER_H_

#include "../ast/ast.h"


void compiler_write_asm(const char *filepath);

void compiler_compile(ast_t *root);
void compile_statement(ast_t *node);
void compiler_compile_compound(ast_t *node);
void compiler_compile_fn_def(ast_t *node);
void compiler_compile_fn_call(ast_t *node);


#endif // __COMPILER_H_
