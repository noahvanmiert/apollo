/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#ifndef __PARSER_H_
#define __PARSER_H_

#include "../lexer/lexer.h"
#include "../ast/ast.h"
#include "../scope/scope.h"

typedef struct {
    lexer_t *lexer;

    token_t *current;
    token_t *prev;
} parser_t;


void parser_init(parser_t *parser, lexer_t *lexer);

ast_t *parser_parse(parser_t *parser, scope_t *scope);
ast_t *parser_parse_statements(parser_t *parser, scope_t *scope);
ast_t *parser_parse_statement(parser_t *parser, scope_t *scope);

ast_t *parser_parse_word(parser_t *parser, scope_t *scope);

ast_t *parser_parse_function(parser_t *parser, scope_t *scope);

#endif // __PARSER_H_