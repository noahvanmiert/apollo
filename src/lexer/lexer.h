#ifndef __LEXER_H_
#define __LEXER_H_

#include <stddef.h>

#include "../token/token.h"


struct Lexer {
	const char *data;
	size_t index;
	char current;
};


struct Token *lexer_get_token(struct Lexer *lexer);


#endif // __LEXER_H_
