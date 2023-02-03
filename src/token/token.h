/*
 *	Made by Noah Van Miert
 *	27/01/23
*/


#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <stddef.h>

enum TokenType {
    TOKEN_WORD,
    TOKEN_STR,
	TOKEN_INT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURL,
    TOKEN_RCURL,
    TOKEN_SEMICOLON,
};


struct CToken {
	char value;

	const char *filepath;
	size_t line;
	size_t col;
};


struct Token {
    enum TokenType type;
    const char *value;

	const char *filepath;
	size_t line;
	size_t col;

};


const char *get_token_str(enum TokenType type);
struct Token *create_token(enum TokenType type, const char *value);


#endif // __TOKEN_H_
