#include "token.h"
#include <assert.h>
#include <stdlib.h>


const char *get_str_from_token(enum TokenType type)
{
    switch (type) {
    case TOKEN_WORD:      return "TOKEN_WORD";
    case TOKEN_STR:       return "TOKEN_STR";
    case TOKEN_LPAREN:    return "TOKEN_LPAREN";
    case TOKEN_RPAREN:    return "TOKEN_RPAREN";
    case TOKEN_LCURL:     return "TOKEN_LCURL";
    case TOKEN_RCURL:     return "TOKEN_RCURL";
    case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
    case TOKEN_END:       return "TOKEN_END";

    default:
	assert(0 && "Unkown type given to get_str_from_token()");
    }
}


struct Token *create_token(enum TokenType type, const char *value)
{
    struct Token *token = malloc(sizeof(struct Token));

    token->type = type;
    token->value = value;

    return token;
}
