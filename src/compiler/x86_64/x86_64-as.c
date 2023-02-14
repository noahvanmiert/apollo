#include "x86_64-as.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


extern void code_section_add(const char *str);
extern void compile_statement(ast_t *node);


void x86_64_compile_fn_def(ast_t *node)
{
	char *template = calloc(strlen(node->function_def_name), sizeof(char));
	MEMORY_CHECK(template);
	sprintf(template, "\t.type %s,@function\n", node->function_def_name);
	code_section_add(template);
	free(template);

    template = calloc(strlen(node->function_def_name) * 2, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "%s:   # @%s\n", node->function_def_name, node->function_def_name);

    const char *stack_frame = "\tpush %rbp\n"
                              "\tmov %rsp, %rbp\n";

    code_section_add(template);
    code_section_add(stack_frame);

    /* compile the function body */
    compile_statement(node->function_def_body);

    free(template);

    code_section_add(
        "\tpop %rbp\n"
        "\tret\n\n"
    );
}

void x86_64_compile_fn_call(ast_t *node)
{
    char *template = calloc(strlen(node->function_call_name) + 5, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "\tcall %s\n", node->function_call_name);

    code_section_add(template);
    free(template);
}
