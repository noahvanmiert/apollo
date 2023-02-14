#include "x86_64-as.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


extern void code_section_add(const char *str);
extern void compile_statement(ast_t *node);


void x86_64_compile_fn_def(ast_t *node)
{
    char *template = calloc(strlen(node->function_def_name) * 2 + 10, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "%s:   ; @%s\n", node->function_def_name, node->function_def_name);

    const char *stack_frame = "\tpush %rbp  ; stack frame\n"
                              "\tmov %rsp, %rbp\n";

    code_section_add(template);
    code_section_add(stack_frame);

    /* compile the function body */
    compile_statement(node->function_def_body);

    free(template);

    code_section_add(
        "\tpop %rbp\n"
        "\tret    ; @"
    );

    template = calloc(strlen(node->function_def_name) + 7, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "%s-end\n\n", node->function_def_name);

    code_section_add(template);
    free(template);
}

void x86_64_compile_fn_call(ast_t *node)
{
    char *template = calloc(strlen(node->function_call_name) + 5, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "\tcall %s\n", node->function_call_name);

    code_section_add(template);
    free(template);
}
