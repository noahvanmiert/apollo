#include <stdio.h>
#include "token/token.h"


int main(void)
{
    struct Token *token = create_token(TOKEN_WORD, "fn");

    printf("token: '%s'\n", token->value);
    printf("token type: '%s'\n", get_str_from_token(token->type));

    return 0;
}
