/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#include "parser.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>


/*
 *  Initializes a parser object.
 *  @param parser: The parser object.
 *  @param lexer:  The lexer object, it's is used to get the tokens.
 *  @return:       Nothing
*/
void parser_init(parser_t *parser, lexer_t *lexer)
{
    parser->lexer = lexer;

    parser->current = lexer_get_token(lexer);
    parser->prev = NULL;
}


/*
 *  Wrapper around parser_parse_statements, this will parse a list of
 *  tokens, the tokens are got from the lexer inside the parser.
 *  @param parser: The parse object.
 *  @param scope:  A scope object, this keeps track of definitions.
 *  @return:       The root AST.
*/
ast_t *parser_parse(parser_t *parser, scope_t *scope)
{
    ast_t *root = parser_parse_statements(parser, scope);
    return root;
}


/*
 *  Check if the current token has the expected type, then advance to the next token.
 *  @param parser:   The parser object.
 *  @param expected: The expected token type (tokentype_t).
 *  @return:         Nothing
*/
static void __consume(parser_t *parser, tokentype_t expected)
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


/*
 *  Add an item to the compound list of an AST.
 *  @param ast:  The ast were the item wil be added to.
 *  @param item: The item that will be added.
 *  @return:     Nothing
*/
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


/*
 *  Parses statements and adds it to the compound list of the root AST.
 *  @param parser: The parser object.
 *  @param scope:  The current scope object.
 *  @return:       The compound AST.
*/
ast_t *parser_parse_statements(parser_t *parser, scope_t *scope)
{
    ast_t *compound = ast_new(AST_COMPOUND);
    
    __compound_list_add(compound, parser_parse_statement(parser, scope));

    while (parser->current->type == TOKEN_SEMICOLON) {
        __consume(parser, TOKEN_SEMICOLON);

		ast_t *statement = parser_parse_statement(parser, scope);

		if (parser->current == NULL)
			break;

        __compound_list_add(compound, statement);
    }

    return compound;
}


/*
 *  Parses an individual statement.
 *  @param parser: The parser object.
 *  @param scope:  The current scope object.
 *  @return:       The Ast generated for the statement.
*/
ast_t *parser_parse_statement(parser_t *parser, scope_t *scope)
{
	if (parser->current == NULL)
		return ast_new(AST_NOP);

    switch (parser->current->type) {
        case TOKEN_WORD:  return parser_parse_word(parser, scope);
        case TOKEN_RCURL: return ast_new(AST_NOP);
        case TOKEN_END:   return ast_new(AST_NOP);
    
        default: assert(0 && "unreachable in parser_parse_statement()");
    }
}


/*
 *  Parses a word, like 'func' for function definitions.
 *  @param parser: The parser object.
 *  @param scope:  The current scope object.
 *  @return:       The AST generated for the statement or definition.
*/
ast_t *parser_parse_word(parser_t *parser, scope_t *scope)
{
    if (strcmp(parser->current->value, "func") == 0)
        return parser_parse_fn_def(parser, scope);

    return parser_parse_fn_call(parser, scope);
}


/*
 *  Parses a funtion definition.
 *  @param parser: The parser object.
 *  @param scope:  The current scope object.
 *  @return:       The AST containing the function definiton.
*/
ast_t *parser_parse_fn_def(parser_t *parser, scope_t *scope)
{
    ast_t *fn_def = ast_new(AST_FUNCTION_DEF);

    __consume(parser, TOKEN_WORD);

    fn_def->function_def_name = parser->current->value;
    __consume(parser, TOKEN_WORD);

    __consume(parser, TOKEN_LPAREN);
    __consume(parser, TOKEN_RPAREN);
    __consume(parser, TOKEN_LCURL);

    // TODO: check if function is already defined

    /* 
        TODO: change scope the a new scope
        and copy all the content into the new one
    */
    fn_def->function_def_body = parser_parse_statements(parser, scope);

    /*
        We need to manualy check if the current token is }
        because if we __consume() it we can't parse the next token
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

    parser->current->type = TOKEN_SEMICOLON;

    scope_add_function(scope, fn_def);

    return fn_def;
}


/*
 *  Parses a function call.
 *  @param parser: The parser object.
 *  @param scope:  The current scope object.
 *  @return:       The AST containing the function call.
*/
ast_t *parser_parse_fn_call(parser_t *parser, scope_t *scope)
{
    ast_t *fn_call = ast_new(AST_FUNCTION_CALL);

    fn_call->function_call_name = parser->current->value;
    __consume(parser, TOKEN_WORD);


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

    __consume(parser, TOKEN_LPAREN);
    __consume(parser, TOKEN_RPAREN);

    return fn_call;
}


// TODO: check for semicolon after statement
