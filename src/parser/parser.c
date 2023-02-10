/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#include "parser.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>


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
    switch (parser->current->type)
    {
    case TOKEN_WORD: return parser_parse_word(parser, scope);
    
    default: assert(0 && "unreachable in parser_parse_statement()");
    }
}


ast_t *parser_parse_word(parser_t *parser, scope_t *scope)
{
    if (strcmp(parser->current->value, "func") == 0)
        return parser_parse_function(parser, scope);

    return NULL;
}


ast_t *parser_parse_function(parser_t *parser, scope_t *scope)
{
    ast_t *fn_def = ast_new(AST_FUNCTION_DEF);

    fn_def->function_def_name = parser->current->value;
    eat(parser, TOKEN_WORD);

    eat(parser, TOKEN_LPAREN);
    eat(parser, TOKEN_RPAREN);
    eat(parser, TOKEN_LCURL);

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

    return fn_def;
}
