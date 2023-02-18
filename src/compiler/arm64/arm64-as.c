/*
 *	Made by Noah Van Miert
 *	12/02/2023
*/

#include "arm64-as.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


extern void code_section_add(const char *str);
extern void compile_statement(ast_t *node);


// TODO: design a system that keeps track of the amount of bytes
// that are stack-allocated so we need to know how much we need to 
// subtract from the stack-pointer in the stack-frame.


/*
 *  Returns the correct stackframe size for arm64, this is needed because the size needs to be divisble by 8.
 *  @param allocated_size: The amount of bytes that needs to be allocated.
 *  @return:               The correct frame size.
*/
static size_t __get_sf_size(size_t alloced_size)
{
    assert(alloced_size < 504);

    if (alloced_size < 16)  return 16;
    if (alloced_size < 32)  return 32;
    if (alloced_size < 64)  return 64;
    if (alloced_size < 128) return 128;
    if (alloced_size < 256) return 256;
    if (alloced_size < 504) return 504;

    assert(0);
}


static size_t __sf_size = 0;


/*
 *  Generates specific assembly code for function definition for the arm64 platform.
 *  @param: The function definition AST.
*/
void arm64_compile_fn_def(ast_t *node)
{
    __sf_size = __get_sf_size(node->stackframe.frame_size);

    char *template = calloc(strlen(node->function_def_name) * 2 + 10, sizeof(char));
    MEMORY_CHECK(template);
    sprintf(template, "%s:    ; @%s\n", node->function_def_name, node->function_def_name);
    code_section_add(template);

    const char *stack_frame = "\tstp     x29, x30, [sp, -%zu]!\n"
                              "\tmov     x29, sp\n";

    template = realloc(template, (strlen(stack_frame) + 1 + 10) * sizeof(char));
    MEMORY_CHECK(template);

    if (node->stackframe.frame_size != 0)
        sprintf(template, stack_frame, __get_sf_size(node->stackframe.frame_size));
    else
        sprintf(template, stack_frame, 32);

    code_section_add(template);

    /* compile the function body */
    compile_statement(node->function_def_body);

    stack_frame = "\tldp     x29, x30, [sp], %zu\n"
                  "\tret     ; @";

    template = realloc(template, (strlen(stack_frame) + 10) * sizeof(char));
    MEMORY_CHECK(template);

    if (node->stackframe.frame_size != 0)
        sprintf(template, stack_frame, __get_sf_size(node->stackframe.frame_size));
    else
        sprintf(template, stack_frame, 32);

    code_section_add(template);

    template = realloc(template, (strlen(node->function_def_name) + 7) * sizeof(char));
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
    sprintf(template, "\tbl      %s\n", node->function_call_name);

    code_section_add(template);

    free(template);
}


/*
 *  Generates specific assembly code for variable definitions for the arm64 platform.
 *  @param node: The variable definition AST.
*/
void arm64_compile_var_def(ast_t *node)
{
    if (node->variable_def_value->type == AST_UINT32) {
        char *template = calloc(14 + 10, sizeof(char));
        MEMORY_CHECK(template);
        sprintf(template, "\tmov     w0, %d\n", node->variable_def_value->uint32_value);
        code_section_add(template);

        template = realloc(template, (20 + 10) * sizeof(char));
        MEMORY_CHECK(template);
        sprintf(template, "\tstr     w0, [sp, %lu]\n", __sf_size - node->variable_offset);
        code_section_add(template);

        free(template);

        return;
    }

    assert(0 && "unsupported datatype");
}
