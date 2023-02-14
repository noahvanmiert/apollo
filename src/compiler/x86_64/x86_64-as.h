#ifndef __X86_64_AS_H_
#define __X86_64_AS_H_

#include "../../ast/ast.h"

#define x86_64_setup_code      "; Generated by the Apollo compiler\n\n" \
                               ".text\n" \
                               ".globl _start\n\n" \
                               "\t.type _start,@function\n" \
                               "_start:\n" \
                               "\tcall main\n" \
                               "\tmov $60, %rax\n" \
                               "\tmov $0, %rdi\n" \
                               "\tint $0x80\n\n"

void x86_64_compile_fn_def(ast_t *node);
void x86_64_compile_fn_call(ast_t *node);

#endif // __X86_64_AS_H_