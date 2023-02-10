#ifndef __SCOPE_H_
#define __SCOPE_H_

#include "../ast/ast.h"

typedef struct {
    ast_t **function_defs;
    size_t function_defs_size;
} scope_t;


void scope_init(scope_t *scope);

void scope_add_function(scope_t *scope, ast_t *function);
ast_t *scope_get_function(scope_t *scope, const char *name);

#endif // __SCOPE_H_
