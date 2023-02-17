/*
 *	Made by Noah Van Miert
 *	27/01/23
*/


#include "token.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


const char *get_token_str(tokentype_t type)
{
    switch (type) {
		case TOKEN_WORD:      return "TOKEN_WORD";
		case TOKEN_STR:       return "TOKEN_STR";
		case TOKEN_CHAR:	  return "TOKEN_CHAR";
		case TOKEN_INT:		  return "TOKEN_INT";
		case TOKEN_LPAREN:    return "TOKEN_LPAREN";
		case TOKEN_RPAREN:    return "TOKEN_RPAREN";
		case TOKEN_LCURL:     return "TOKEN_LCURL";
		case TOKEN_RCURL:     return "TOKEN_RCURL";
		case TOKEN_COLON:     return "TOKEN_COLON";
		case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
		case TOKEN_EQ:		  return "TOKEN_EQ";
		case TOKEN_END:		  return "TOKEN_END";

		default:
			assert(0 && "Unkown type given to get_token_str()");
    }
}


token_t *token_new(tokentype_t type, const char *value)
{
    token_t *token = malloc(sizeof(token_t));
    
	if (!token) {
		fprintf(stderr, "error: memory allocation failed\n");
		exit(1);
	}	

    token->type = type;
    token->value = value;

	token->filepath = NULL;
	token->line = 0;
	token->col = 0;

    return token;
}
