/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#ifndef __PARSER_H_
#define __PARSER_H_

#include "../lexer/lexer.h"
#include "../ast/ast.h"
#include "../scope/scope.h"

#include <stdbool.h>


typedef struct {
    lexer_t *lexer;

    token_t *current;
    token_t *prev;

    bool in_fn_def;
    size_t variable_offset;
} parser_t;


void parser_init(parser_t *parser, lexer_t *lexer);

ast_t *parser_parse(parser_t *parser, scope_t *scope);
ast_t *parser_parse_statements(parser_t *parser, scope_t *scope);
ast_t *parser_parse_statement(parser_t *parser, scope_t *scope);
ast_t *parser_parse_expr(parser_t *parser);

ast_t *parser_parse_word(parser_t *parser, scope_t *scope);
ast_t *parser_parse_uint32(parser_t *parser);

ast_t *parser_parse_fn_def(parser_t *parser, scope_t *scope);
ast_t *parser_parse_fn_call(parser_t *parser, scope_t *scope);

ast_t *parser_parse_var_def(parser_t *parser, scope_t *scope);

#endif // __PARSER_H_
