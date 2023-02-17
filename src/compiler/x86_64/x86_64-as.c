/*
 *	Made by Noah Van Miert
 *	13/02/2023
*/

#include "x86_64-as.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


extern void code_section_add(const char *str);
extern void compile_statement(ast_t *node);


/*
 *  Generates specific assembly code for function definition for the x64 platform.
 *  @param: The function definition AST.
*/
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


/*
 *  Generates specific assembly code for function calls for the x64 platform.
 *  @param node: The function call AST.
*/
void x86_64_compile_fn_call(ast_t *node)
{
    char *template = calloc(strlen(node->function_call_name) + 5, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "\tcall %s\n", node->function_call_name);

    code_section_add(template);
    free(template);
}


/*
 *  Generated specific assemblt code for variable definitions for the x64 platform.
 *  @param node: The variable definition AST.
*/
void x86_64_compile_var_def(ast_t *node)
{
    if (node->variable_def_value->type == AST_UINT32) {
        char *template = calloc(30, sizeof(char));
        MEMORY_CHECK(template);
        sprintf(template, "\tmov $%d, -%zu(%%rbp)\n", node->variable_def_value->uint32_value, node->variable_offset);
        code_section_add(template);
        free(template);

        return;
    }

    assert(0 && "unkown variable type");
}
