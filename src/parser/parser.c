/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#include "parser.h"

void parser_init(parser_t *parser, lexer_t *lexer)
{
    parser->lexer = lexer;

    parser->current = lexer_get_token(lexer);
    parser->prev = NULL;
}
