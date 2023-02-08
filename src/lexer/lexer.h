/*
 *	Made by Noah Van Miert
 *	31/01/2023
*/


#ifndef __LEXER_H_
#define __LEXER_H_


#include "../token/token.h"
#include <stddef.h>

typedef struct {
	const char *filepath;

	ctoken_t *data;
	ctoken_t current;
	size_t index;
} lexer_t;


void lexer_init(lexer_t *lexer, const char *filepath);

token_t *lexer_get_token(lexer_t *lexer);

void apo_compiler_err(const char *filepath, size_t line, size_t col, const char *fmt, ...);

#endif // __LEXER_H_
