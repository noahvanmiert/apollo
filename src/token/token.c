/*
 *	Made by Noah Van Miert
 *	27/01/23
*/

#include "token.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


/*
 *	Allocates a new token object and initializes it.
 *	@param type:  The type of the token.
 *	@param value: The value of the token.
*/
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
