/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#include "parser.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>


void parser_init(parser_t *parser, lexer_t *lexer)
{
    parser->lexer = lexer;

    parser->current = lexer_get_token(lexer);
    parser->prev = NULL;
}


ast_t *parser_parse(parser_t *parser, scope_t *scope)
{
    return parser_parse_statements(parser, scope);
}


static void eat(parser_t *parser, tokentype_t expected)
{
    if (parser->current->type != expected) {
        apo_compiler_err(
            parser->current->filepath,
            parser->current->line,
            parser->current->col,
            "error: unexpected token '%s'\n",
            parser->current->value
        );
    }

    parser->prev = parser->current;
    parser->current = lexer_get_token(parser->lexer);
}


static void __compound_list_add(ast_t *ast, ast_t *item)
{
    ast->compound_value = realloc(
        ast->compound_value,
        (++ast->compound_size) * sizeof(ast_t *)
    );

    if (ast->compound_value == NULL) {
        fprintf(stderr, "error: memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    ast->compound_value[ast->compound_size - 1] = item;
}


ast_t *parser_parse_statements(parser_t *parser, scope_t *scope)
{
    ast_t *compound = ast_new(AST_COMPOUND);
    
    __compound_list_add(compound, parser_parse_statement(parser, scope));

    while (parser->current->type == TOKEN_SEMICOLON) {
        eat(parser, TOKEN_SEMICOLON);
        __compound_list_add(compound, parser_parse_statement(parser, scope));
    }

    return compound;
}


ast_t *parser_parse_statement(parser_t *parser, scope_t *scope)
{
    switch (parser->current->type) {
        case TOKEN_WORD:  return parser_parse_word(parser, scope);
        case TOKEN_RCURL: return ast_new(AST_NOP);
    
        default: assert(0 && "unreachable in parser_parse_statement()");
    }
}


ast_t *parser_parse_word(parser_t *parser, scope_t *scope)
{
    if (strcmp(parser->current->value, "func") == 0)
        return parser_parse_fn_def(parser, scope);

    return parser_parse_fn_call(parser, scope);
}


ast_t *parser_parse_fn_def(parser_t *parser, scope_t *scope)
{
    ast_t *fn_def = ast_new(AST_FUNCTION_DEF);

    eat(parser, TOKEN_WORD);

    fn_def->function_def_name = parser->current->value;
    eat(parser, TOKEN_WORD);

    eat(parser, TOKEN_LPAREN);
    eat(parser, TOKEN_RPAREN);
    eat(parser, TOKEN_LCURL);

    // TODO: check if function is already defined

    /* 
        TODO: change scope the create a new scope
        and copy all the content into the new one
    */
    fn_def->function_def_body = parser_parse_statements(parser, scope);

    /*
        We need to manualy check if the current token is }
        because if we eat() it we can't parse the next token
        in parser_parse_statements()
    */
    if (parser->current->type != TOKEN_RCURL) {
        apo_compiler_err(
            parser->current->filepath,
            parser->current->line,
            parser->current->col,
            "error: unexpected token '%s', expected '}'\n",
            parser->current->value
        );
    }

    scope_add_function(scope, fn_def);

    return fn_def;
}


ast_t *parser_parse_fn_call(parser_t *parser, scope_t *scope)
{
    ast_t *fn_call = ast_new(AST_FUNCTION_CALL);

    fn_call->function_call_name = parser->current->value;
    eat(parser, TOKEN_WORD);


    /* Check if function exists in current scope. */
    if (scope_get_function(scope, fn_call->function_call_name) == NULL) {
        apo_compiler_err(
            parser->current->filepath,
            parser->current->line,
            parser->current->col,
            "error: undefined function '%s'",
            fn_call->function_call_name
        );
    }


    eat(parser, TOKEN_LPAREN);
    eat(parser, TOKEN_RPAREN);

    return fn_call;
}


// TODO: check for semicolon after statement
