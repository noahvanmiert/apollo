/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#include "parser.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "variables/variable.h"


#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


/*
 *	Prints a given error message, with the format: 'filepath:line:col: message'.
 *	@param parser: The parser object, used for the location of the current token.
*/
static void parser_err(parser_t *parser, const char *fmt, ...)
{
    va_list args;
	va_start(args, fmt);

	printf("\033[1;31m");
	printf("%s:%zu:%zu: ", parser->current->filepath, parser->current->line, parser->current->col);
	vprintf(fmt, args);
	printf("\033[0m\n");

	exit(EXIT_FAILURE);
}


/*
 *  Initializes a parser object.
 *  @param parser: The parser object.
 *  @param lexer:  The lexer object, it's is used to get the tokens.
*/
void parser_init(parser_t *parser, lexer_t *lexer)
{
    parser->lexer = lexer;

    parser->current = lexer_get_token(lexer);
    parser->prev = NULL;

    parser->in_fn_def = false;
    parser->entry_defined = false;

    parser->variable_offset = 0;
    parser->current_sf_size = 0;
}


/*
 *  Returns the datatype corresponding with the AST. type.
 *  @param ast: The AST.
*/
data_type_t get_type_from_ast(ast_t *ast)
{
    switch (ast->type) {
        case AST_UINT32: return TYPE_UINT32;

        default: return TYPE_UNKOWN;
    }
}


/* 
 *  Checks if a given word is a language keyword.
 *  @param word: The word to check.
*/
static bool __is_keyword(const char *word)
{
    if (strcmp(word, "func") == 0)
        return true;

    return false;
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

    if (!parser->entry_defined) {
        fprintf(stderr, "\033[31;1merror: no entry point defined\033[0m\n");
        exit(1);
    }

    return root;
}


/*
 *  Check if the current token has the expected type, then advance to the next token.
 *  @param parser:   The parser object.
 *  @param expected: The expected token type (tokentype_t).
*/
static void __consume(parser_t *parser, tokentype_t expected)
{
    if (parser->current->type != expected)
        parser_err(parser, "error: unexpected token '%s'", parser->current->value);

    parser->prev = parser->current;
    parser->current = lexer_get_token(parser->lexer);
}


/*
 *  Add an item to the compound list of an AST.
 *  @param ast:  The ast were the item wil be added to.
 *  @param item: The item that will be added.
*/
static void __compound_list_add(ast_t *ast, ast_t *item)
{
    ast->compound_value = realloc(
        ast->compound_value,
        (++ast->compound_size) * sizeof(ast_t *)
    );

    MEMORY_CHECK(ast->compound_value);

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
 *  Parses an expression like a string literal, a integer literal, ...
 *  @param parser: The parser object.
*/
ast_t *parser_parse_expr(parser_t *parser)
{
    switch (parser->current->type) {
        case TOKEN_INT: return parser_parse_uint32(parser);

        default: assert(0);
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

    if (strcmp(parser->current->value, "let") == 0)
        return parser_parse_var_def(parser, scope);

    return parser_parse_fn_call(parser, scope);
}


/*
 *  Parses an integer literal.
 *  @param parser: The parser object.
*/
ast_t *parser_parse_uint32(parser_t *parser)
{
    ast_t *number = ast_new(AST_UINT32);
    number->uint32_value = atoi(parser->current->value);

    __consume(parser, TOKEN_INT);

    return number;
}


/*
 *  Parses a funtion definition.
 *  @param parser: The parser object.
 *  @param scope:  The current scope object.
 *  @return:       The AST containing the function definiton.
*/
ast_t *parser_parse_fn_def(parser_t *parser, scope_t *scope)
{
    if (parser->in_fn_def)
        parser_err(parser, "error: defining a function inside another function is illegal");

    ast_t *fn_def = ast_new(AST_FUNCTION_DEF);

    __consume(parser, TOKEN_WORD);

    fn_def->function_def_name = parser->current->value;

    if (__is_keyword(fn_def->function_def_name))
        parser_err(parser, "error: illegal function name '%s', function name cannot be a language keyword", fn_def->function_def_name);

    
    if (!strcmp(parser->current->value, "main") && parser->entry_defined)
        parser_err(parser, "error: entry point is already defined");

    if (!strcmp(parser->current->value, "main"))
        parser->entry_defined = true;


    __consume(parser, TOKEN_WORD);
    __consume(parser, TOKEN_LPAREN);
    __consume(parser, TOKEN_RPAREN);
    __consume(parser, TOKEN_LCURL);

    // TODO: when this function already exists the error message location is at the last brace, change
    // this to the first char of the name

    /* check if function is already defined in this scope */
    if (scope_get_function(scope, fn_def->function_def_name) != NULL)
        parser_err(parser, "error: re-defining a function is illegal");


    /* 
        TODO: change scope the a new scope
        and copy all the content into the new one
    */

    parser->in_fn_def = true;
    parser->current_sf_size = 0;
    fn_def->function_def_body = parser_parse_statements(parser, scope);

    stack_frame_t sf;
    sf.frame_size = parser->current_sf_size;
    fn_def->stackframe = sf;

    parser->in_fn_def = false;

    /*
        We need to manualy check if the current token is }
        because if we __consume() it we can't parse the next token
        in parser_parse_statements()
    */
    if (parser->current->type != TOKEN_RCURL) 
        parser_err(parser, "error: unexpected token '%s', expected '}'", parser->current->value);

    parser->current->type = TOKEN_SEMICOLON;

    scope_add_function(scope, fn_def);

    return fn_def;
}


ast_t *parser_parse_variable_redef(parser_t *parser, scope_t *scope)
{
    ast_t *var_redef = ast_new(AST_VARIABLE_REDEF);
    var_redef->variable_redef_name = parser->prev->value;

    ast_t *var_def = scope_get_variable(scope, var_redef->variable_redef_name);


    if (!var_def)
        parser_err(parser, "error: undefined variable '%s'", var_redef->variable_name);

    
    var_redef->variable_redef_name = var_def->variable_def_name;
    var_redef->variable_offset = var_def->variable_offset;

    __consume(parser, TOKEN_EQ);

    var_redef->variable_redef_value = parser_parse_expr(parser);


    if (var_def->variable_def_type != get_type_from_ast(var_redef->variable_redef_value))
        parser_err(parser, "error: the value type does not match the type of variable '%s'", var_def->variable_def_name);
        

    return var_redef;    
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


    if (parser->current->type == TOKEN_EQ)
        return parser_parse_variable_redef(parser, scope);


    /* Check if function exists in current scope. */
    if (scope_get_function(scope, fn_call->function_call_name) == NULL)
        parser_err(parser, "error: calling undefined function '%s'", fn_call->function_call_name);


    __consume(parser, TOKEN_LPAREN);
    __consume(parser, TOKEN_RPAREN);

    return fn_call;
}


/*
 *  Parses a variable definition.
 *  @param parser: The parser object.
 *  @param scope: The scope object.
*/
ast_t *parser_parse_var_def(parser_t *parser, scope_t *scope)
{
    __consume(parser, TOKEN_WORD);

    ast_t *var_def = ast_new(AST_VARIABLE_DEF);
    var_def->variable_def_name = parser->current->value;


    /* Check if the variable name is a keyword, this is illegal so we just print an error */
    if (__is_keyword(parser->current->value))
        parser_err(parser, "error: illegal function name '%s', variable name cannot be a language keyword", parser->current->value);


    /* Check if the variable is already defined in the scope */
    if (scope_get_variable(scope, parser->current->value) != NULL) 
        parser_err(parser, "error: re-defining a variable is illegal");


    __consume(parser, TOKEN_WORD);
    __consume(parser, TOKEN_COLON);

    data_type_t var_type = get_type_from_str(parser->current->value);


    /* Check if the variable type is valid */
    if (var_type == TYPE_UNKOWN)
        parser_err(parser, "error: unkown variable type: '%s'", parser->current->value);

    __consume(parser, TOKEN_WORD);


    /* if you just declare a variable 'let a: uint32;' it will be initialized with 0, unlike in c */
    if (parser->current->type == TOKEN_SEMICOLON) {
        var_def->variable_def_value = ast_new(AST_UINT32);
        var_def->variable_def_value->uint32_value = 0;
        goto out;
    }


    __consume(parser, TOKEN_EQ);

    var_def->variable_def_value = parser_parse_expr(parser);
    var_def->variable_def_type = var_type;
    

    /* check if the given type and the type of the value match, else we print an error */
    if (get_type_from_ast(var_def->variable_def_value) != var_type)
        parser_err(parser, "error: variable value type does not match the given type");

out:
    parser->variable_offset += get_type_size(var_type);

    var_def->variable_offset = parser->variable_offset;
    parser->current_sf_size += parser->variable_offset;

    scope_add_variable(scope, var_def);

    return var_def;
}


// TODO: check for semicolon after statement
