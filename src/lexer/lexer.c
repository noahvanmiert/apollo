/*
 *	Made by Noah Van Miert
 *	31/01/2023
*/


#include "lexer.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static inline void advance(struct Lexer *lexer)
{
	lexer->index++;
	lexer->current = lexer->data[lexer->index];
}


static inline void skip_white(struct Lexer *lexer)
{
	while (isspace(lexer->current))
		advance(lexer);
}


static struct Token *parse_special(struct Lexer *lexer)
{
	switch (lexer->current) {
		case '(': advance(lexer); return create_token(TOKEN_LPAREN, "(");
		case ')': advance(lexer); return create_token(TOKEN_RPAREN, ")");
		case '{': advance(lexer); return create_token(TOKEN_LCURL, "{");
		case '}': advance(lexer); return create_token(TOKEN_RCURL, "}");
		case ';': advance(lexer); return create_token(TOKEN_SEMICOLON, ";");

		default: {
			fprintf(stderr, "\033[31;1merror: unkown character '%c' with ASCII-code '%d'\n\033[0m", lexer->current, (int) lexer->current);
			exit(EXIT_FAILURE);
		}
	}
}


static struct Token *parse_word(struct Lexer *lexer)
{
	char *word = calloc(1, sizeof(char));

	if (word == NULL)
		assert(0 && "error: apollo compiler could not allocate enough memory");

	while (isalpha(lexer->current)) {
		word = realloc(word, (strlen(word) + 2) * sizeof(char));
		strcat(word, (char []) {lexer->current, '\0'});
		advance(lexer);
	}

	return create_token(TOKEN_WORD, word);
}


struct Token *lexer_get_token(struct Lexer *lexer)
{
	if (lexer->current != '\0') {
		skip_white(lexer);

		if (isalpha(lexer->current))
			return parse_word(lexer);

		return parse_special(lexer);
	}

	return NULL;
}


// TODO: use a linked list instead of a normal array to store the tokens
