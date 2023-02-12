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
                                     "\tcall main\n"
                                     "\tmov X0, #0\n"
                                     "\tmov X16, 1\n"
                                     "\tsvc #0x80\n";


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
        case AST_COMPOUND:     compiler_compile_compound(node); break;
        case AST_FUNCTION_DEF: compiler_compile_fn_def(node);   break;
        case AST_FUNCTION_CALL: compiler_compile_fn_call(node); break;
        case AST_NOP:          return;

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
    char *template = calloc(strlen(node->function_def_name) + 4, sizeof(char));
    sprintf(template, "%s:\n", node->function_def_name);
    const char *stack_frame = "\tsub sp, sp, #16\n"
                              "\tstr w0, [sp, #12]\n";

    code_section_add(template);
    code_section_add(stack_frame);

    free(template);

    code_section_add("\tret\n");
}


void compiler_compile_fn_call(ast_t *node)
{
    char *template = calloc(strlen(node->function_call_name) + 5, sizeof(char));
    sprintf(template, "bl %s\n", node->function_call_name);

    code_section_add(template);
}
