/*
 *	Made by Noah Van Miert
 *	11/02/2023
*/

#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* for arm64 specific assembly */
#include "arm64/arm64-as.h"

/* for x86_64 specific assembly */
#include "x86_64/x86_64-as.h"

#include "../apollo/apollo.h"


#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


static char *code_section = NULL;
extern flag_info_t flag_info;


/*
 *  Adds a string to the code section of the assembly.
 *  @param str: The string that will be added to the code section.
*/
void code_section_add(const char *str)
{
    size_t size = strlen(str);

    if (code_section == NULL) {
        code_section = malloc((size + 1) * sizeof(char));
    } else {
        /* allocate an extra byte '+ 1' for the '\0' character */
        code_section = realloc(code_section, (strlen(code_section) + size + 1) * sizeof(char));
    }
    
    MEMORY_CHECK(code_section);
    strcat(code_section, str);

    code_section[strlen(code_section)] = '\0'; 
}


/*
 *  Writes the generated to a file.
 *  @param filepath: The filepath of the output file.
*/
void compiler_write_asm(const char *filepath)
{
    FILE *fptr = fopen(filepath, "w");

    if (!fptr) {
        fprintf(stderr, "error: could not open file");
        exit(1);
    }

    switch (flag_info.target) {
        case PLATFORM_MAC_ARM64: {
            fputs(arm64_setup_code, fptr);
            break;
        }

        case PLATFORM_LINUX_X64: {
            fputs(linux_x64_setup_code, fptr);
            break;
        }

        default: assert(0 && "unkown platform");
    }

    
    fputs(code_section, fptr);

    fclose(fptr);
}


/*
 *  Wrapper around compile_statement().
 *  @param root: The root AST.
*/
void compiler_compile(ast_t *root)
{
    compile_statement(root);
}


/*
 *  Compiles a statements, it calls other functions based on the type of the node.
 *  @param node: The statement.
*/
void compile_statement(ast_t *node)
{
    switch (node->type) {
        case AST_COMPOUND:      compiler_compile_compound(node); break;
        case AST_FUNCTION_DEF:  compiler_compile_fn_def(node);   break;
        case AST_FUNCTION_CALL: compiler_compile_fn_call(node);  break;
        case AST_VARIABLE_DEF:  compiler_compile_var_def(node);  break;
        case AST_NOP:           return;

        default: assert(0);
    }
}


/*
 *  Compiles a compound AST.
 *  @param node: The compound AST.
*/
void compiler_compile_compound(ast_t *node)
{
    for (size_t i = 0; i < node->compound_size; i++)
        compile_statement(node->compound_value[i]);
}


/*
 *  Compiles a function definition.
 *  @param node: The function definition AST.
*/
void compiler_compile_fn_def(ast_t *node)
{
    /* call the function associated with the platform */
    switch (flag_info.target) {
        case PLATFORM_MAC_ARM64: {
            arm64_compile_fn_def(node);
            break;
        }

        case PLATFORM_LINUX_X64: {
            x86_64_compile_fn_def(node);
            break;
        }

        default: assert(0 && "unkown platform");
    }
}


/*
 *  Compiles a function call.
 *  @param node: The function call AST.
*/
void compiler_compile_fn_call(ast_t *node)
{
    /* call the function associated with the platform */
    switch (flag_info.target) {
        case PLATFORM_MAC_ARM64: {
            arm64_compile_fn_call(node);
            break;
        }

        case PLATFORM_LINUX_X64: {
            x86_64_compile_fn_call(node);
            break;
        }

        default: assert(0 && "unkown platform");
    }
}


/*
 *  Compiles a variable definition.
 *  @param node: The variable definition AST.
*/
void compiler_compile_var_def(ast_t *node)
{
    switch (flag_info.target) {
        case PLATFORM_MAC_ARM64: {
            arm64_compile_var_def(node);
            break;
        }

        case PLATFORM_LINUX_X64: {
            x86_64_compile_var_def(node);
            break;
        }

        default: assert(0 && "unkown platform");
    }
}
