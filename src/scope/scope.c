#include "scope.h"
#include <stdlib.h>
#include <string.h>

void scope_init(scope_t *scope)
{
    scope->function_defs = NULL;
    scope->function_defs_size = 0;
}


void scope_add_function(scope_t *scope, ast_t *function)
{
    scope->function_defs_size++;

    scope->function_defs = realloc(scope->function_defs, scope->function_defs_size * sizeof(scope_t));
    scope->function_defs[scope->function_defs_size - 1] = function;
}


ast_t *scope_get_function(scope_t *scope, const char *name)
{
    for (size_t i = 0; i < scope->function_defs_size; i++) {
        if (strcmp(scope->function_defs[i]->function_def_name, name) == 0)
            return scope->function_defs[i];
    }

    return NULL;
}
