/*
 *	Made by Noah Van Miert
 *	9/02/2023
*/

#ifndef __PARSER_H_
#define __PARSER_H_

#include "../lexer/lexer.h"


typedef struct {
    lexer_t *lexer;

    token_t *current;
    token_t *prev;
} parser_t;


void parser_init(parser_t *parse, lexer_t *lexer);


#endif // __PARSER_H_
