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

	assert(word && "error: apollo compiler could not allocate enough memory");


	while (isalpha(lexer->current)) {
		word = realloc(word, (strlen(word) + 2) * sizeof(char));

		/* Add the current character to the word */
		strcat(word, (char []) {lexer->current, '\0'});
		advance(lexer);
	}


	return create_token(TOKEN_WORD, word);
}


static struct Token *parse_str(struct Lexer *lexer)
{
	char *str = calloc(1, sizeof(char));

	assert(str && "error: apollo compiler could not allocate enough memory");

	/* To jump over the first '"' */
	advance(lexer);


	while (lexer->current != '"') {
		str = realloc(str, (strlen(str) + 2) * sizeof(char));

		/* Add the current character to the string literal */
		strcat(str, (char []) {lexer->current, '\0'});
		advance(lexer);
	}


	/* To jump over the last '"' */
	advance(lexer);

	return create_token(TOKEN_STR, str);
}


static struct Token *parse_number(struct Lexer *lexer)
{
	char *number = calloc(1, sizeof(char));
	
	assert(number && "error: apollo compiler could not allocate enough memory");


	while (isalnum(lexer->current)) {
		number = realloc(number, (strlen(number) + 2) * sizeof(char));

		/* Add the current character to the number literal */
		strcat(number, (char []) {lexer->current, '\0'});
		advance(lexer);
	}


	return create_token(TOKEN_INT, number);
}


struct Token *lexer_get_token(struct Lexer *lexer)
{
	if (lexer->current != '\0') {
		skip_white(lexer);
	
		/* Parse a full word */
		if (isalpha(lexer->current))
			return parse_word(lexer);


		/* Parse a number literal */
		if (isalnum(lexer->current))
			return parse_number(lexer);


		/* Parse a string literal */
		if (lexer->current == '"')
			return parse_str(lexer);


		/* Parse a char like '{', '}', ... */
		return parse_special(lexer);
	}

	return NULL;
}


// TODO: use a linked list instead of a normal array to store the tokens
