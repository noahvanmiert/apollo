/*
 *	Made by Noah Van Miert
 *	31/01/2023
*/


#ifndef __LEXER_H_
#define __LEXER_H_


#include "../token/token.h"
#include <stddef.h>

struct Lexer {
	const char *filepath;
	struct CToken *data;
	size_t index;
	struct CToken current;
};


void lexer_init(struct Lexer *lexer, const char *filepath);
struct Token *lexer_get_token(struct Lexer *lexer);

void apo_compiler_err(const char *filepath, size_t line, size_t col, const char *fmt, ...);

#endif // __LEXER_H_
