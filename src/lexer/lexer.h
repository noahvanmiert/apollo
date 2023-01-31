/*
 *	Made by Noah Van Miert
 *	31/01/2023
*/


#ifndef __LEXER_H_
#define __LEXER_H_


#include "../token/token.h"
#include <stddef.h>

struct Lexer {
	const char *data;
	size_t index;
	char current;
};


struct Token *lexer_get_token(struct Lexer *lexer);


#endif // __LEXER_H_
