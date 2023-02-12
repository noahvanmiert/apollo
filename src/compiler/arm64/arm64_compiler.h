/*
 *	Made by Noah Van Miert
 *	12/02/2023
*/

#ifndef __ARM64_COMPILER_H_
#define __ARM64_COMPILER_H_

#include "../../ast/ast.h"

#define arm64_setup_code       ".global _start\n" \
                               ".align 2\n" \
                               "_start:\n" \
                               "\tbl main\n" \
                               "\tmov X0, #0\n" \
                               "\tmov X16, 1\n" \
                               "\tsvc #0x80\n\n"

void arm64_compile_fn_def(ast_t *node);
void arm64_compile_fn_call(ast_t *node);

#endif // __ARM64_COMPILER_H_