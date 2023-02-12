/*
 *	Made by Noah Van Miert
 *	11/02/2023
*/

#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


static const char *nasm_setup_code = ".global _start\n"
                                     ".align 2\n"
                                     "_start:\n"
                                     "\tbl main\n"
                                     "\tmov X0, #0\n"
                                     "\tmov X16, 1\n"
                                     "\tsvc #0x80\n\n";


static char *code_section = NULL;


void code_section_add(const char *str)
{
    size_t size = strlen(str);

    if (code_section == NULL) {
        code_section = malloc((size + 1) * sizeof(char));
    } else {
        /* allocate an extra byte '+ 1' for the '\0' character */
        code_section = realloc(code_section, (strlen(code_section) + size + 1) * sizeof(char));
    }

    strcat(code_section, str);

    code_section[strlen(code_section)] = '\0'; 
}


void compiler_write_asm(const char *filepath)
{
    FILE *fptr = fopen(filepath, "w");

    if (!fptr) {
        fprintf(stderr, "error: could not open file");
        exit(1);
    }

    fputs(nasm_setup_code, fptr);
    fputs(code_section, fptr);

    fclose(fptr);
}


void compiler_compile(ast_t *root)
{
    compile_statements(root);
}


void compile_statements(ast_t *node)
{
    switch (node->type) {
        case AST_COMPOUND:      compiler_compile_compound(node); break;
        case AST_FUNCTION_DEF:  compiler_compile_fn_def(node);   break;
        case AST_FUNCTION_CALL: compiler_compile_fn_call(node);  break;
        case AST_NOP:           return;

        default: assert(0);
    }
}


void compiler_compile_compound(ast_t *node)
{
    for (size_t i = 0; i < node->compound_size; i++)
        compile_statements(node->compound_value[i]);
}


void compiler_compile_fn_def(ast_t *node)
{
    char *template = calloc(strlen(node->function_def_name) * 2 + 10, sizeof(char));
    sprintf(template, "%s:   ; @%s\n", node->function_def_name, node->function_def_name);

    const char *stack_frame = "\tstp x29, x30, [sp, #-16]!  ; 16-byte folded spill\n"
                              "\tmov x29, sp\n";

    code_section_add(template);
    code_section_add(stack_frame);

    /* compile the function body */
    compile_statements(node->function_def_body);

    free(template);

    code_section_add(
        "\tldp x29, x30, [sp], #16  ; 16-byte folded reload\n"
        "\tret    ; @"
    );

    template = calloc(strlen(node->function_def_name) + 7, sizeof(char));
    sprintf(template, "%s-end\n\n", node->function_def_name);

    code_section_add(template);
    free(template);
}


void compiler_compile_fn_call(ast_t *node)
{
    printf("called\n");
    char *template = calloc(strlen(node->function_call_name) + 5, sizeof(char));
    sprintf(template, "\tbl %s\n", node->function_call_name);

    code_section_add(template);

    free(template);
}
