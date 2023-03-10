/*
 *	Made by Noah Van Miert
 *	13/02/2023
*/

#ifndef __LINUX_X64_AS_H_
#define __LINUX_X64_AS_H_

#include "../../ast/ast.h"


#define linux_x64_setup_code   "# Generated by the Apollo compiler\n\n" \
                               ".globl _start\n" \
                               ".text\n\n" \
                               "\t.type _start,@function\n" \
                               "_start:\n" \
                               "\tcall    main\n" \
                               "\tmov     $60,  %rax\n" \
                               "\txor     %rdi, %rdi\n" \
                               "\tsyscall\n\n"


void x64_compile_fn_def(ast_t *node);
void x64_compile_fn_call(ast_t *node);
void x64_compile_var_def(ast_t *node);
void x64_compile_var_redef(ast_t *node);


#endif // __LINUX_X64_AS_H_
