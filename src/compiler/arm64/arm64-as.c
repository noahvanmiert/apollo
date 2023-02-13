/*
 *	Made by Noah Van Miert
 *	12/02/2023
*/

#include "arm64-as.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


extern void code_section_add(const char *str);
extern void compile_statement(ast_t *node);


/*
 *  Generates specific assembly code for function definition for the arm64 platform.
 *  @param: The function definition AST.
*/
void arm64_compile_fn_def(ast_t *node)
{
    char *template = calloc(strlen(node->function_def_name) * 2 + 10, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "%s:   ; @%s\n", node->function_def_name, node->function_def_name);

    const char *stack_frame = "\tstp x29, x30, [sp, #-16]!  ; 16-byte folded spill\n"
                              "\tmov x29, sp\n";

    code_section_add(template);
    code_section_add(stack_frame);

    /* compile the function body */
    compile_statement(node->function_def_body);

    free(template);

    code_section_add(
        "\tldp x29, x30, [sp], #16  ; 16-byte folded reload\n"
        "\tret    ; @"
    );

    template = calloc(strlen(node->function_def_name) + 7, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "%s-end\n\n", node->function_def_name);

    code_section_add(template);
    free(template);
}


/*
 *  Generates specific assembly code for function calls for the arm64 platform.
 *  @param node: The function call AST.
*/
void arm64_compile_fn_call(ast_t *node)
{
    char *template = calloc(strlen(node->function_call_name) + 5, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "\tbl %s\n", node->function_call_name);

    code_section_add(template);

    free(template);
}
