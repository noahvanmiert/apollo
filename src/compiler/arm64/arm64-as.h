/*
 *	Made by Noah Van Miert
 *	12/02/2023
*/

#ifndef __ARM64_COMPILER_H_
#define __ARM64_COMPILER_H_

#include "../../ast/ast.h"


#define arm64_setup_code       "; Generated by the Apollo compiler\n\n" \
                               ".text\n" \
                               ".global _start\n" \
                               ".align 2\n" \
                               "_start:\n" \
                               "\tbl      main\n" \
                               "\tmov     x0,  0\n" \
                               "\tmov     x16, 1\n" \
                               "\tsvc     0x80\n\n"
                               

void arm64_compile_fn_def(ast_t *node);
void arm64_compile_fn_call(ast_t *node);
void arm64_compile_var_def(ast_t *node);


#endif // __ARM64_COMPILER_H_
