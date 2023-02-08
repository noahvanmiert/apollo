/*
 *	Made by Noah Van Miert
 *	27/01/23
*/


#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <stddef.h>

typedef enum {
    TOKEN_WORD,
    TOKEN_KEYWORD,
    TOKEN_STR,
	TOKEN_CHAR,
	TOKEN_INT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURL,
    TOKEN_RCURL,
    TOKEN_SEMICOLON,
} tokentype_t;


typedef struct {
	char value;

	const char *filepath;
	size_t line;
	size_t col;
} ctoken_t;


typedef struct {
    tokentype_t type;
    const char *value;

	const char *filepath;
	size_t line;
	size_t col;
} token_t;


const char *get_token_str(tokentype_t type);
token_t *token_new(tokentype_t type, const char *value);


#endif // __TOKEN_H_
