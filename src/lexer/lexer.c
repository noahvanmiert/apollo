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
#include <stdarg.h>


#define FILE_BUFFER_CAP 4096
static_assert(FILE_BUFFER_CAP > 0, "error: FILE_BUFFER_CAP should always be higher then zero");


void apo_compiler_err(const char *filepath, size_t line, size_t col, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	printf("\033[1;31m");
	printf("%s: %zu: %zu: ", filepath, line, col);
	vprintf(fmt, args);
	printf("\033[0m\n");

	exit(EXIT_FAILURE);
}


struct CToken *read_file(const char *filepath)
{
	FILE *fptr = fopen(filepath, "r");

	if (!fptr) {
		fprintf(stderr, "\033[1;31merror: could not open file '%s'\033[0m\n", filepath);
		exit(1);
	}

	struct CToken *buffer = malloc(FILE_BUFFER_CAP * sizeof(struct CToken));
	
	if (!buffer) {
		fprintf(stderr, "error: memory allocation failed\n");
		exit(1);
	}

	size_t buffer_size = FILE_BUFFER_CAP;
	size_t buffer_index = 0;
	size_t line = 1, col = 1;

	while (1) {
		if (buffer_index >= buffer_size) {
			buffer_size += FILE_BUFFER_CAP;
			buffer = realloc(buffer, buffer_size * sizeof(struct CToken));

			if (!buffer) {
				fprintf(stderr, "error: memory allocation failed\n");
				exit(1);
			}
		}

		char c = fgetc(fptr);
		if (c == EOF) break;

		buffer[buffer_index].filepath = filepath;
		buffer[buffer_index].line = line;
		buffer[buffer_index].col = col;
		buffer[buffer_index].value = c;

		buffer_index++;


		if (c == '\n') {
			line++;
			col = 1;
		} else {
			col++;
		}
	}

	fclose(fptr);

	buffer = realloc(buffer, (buffer_index + 1) * sizeof(struct CToken));

	if (!buffer) {
		fprintf(stderr, "error: memory allocation failed\n");
		exit(1);
	}

	buffer[buffer_index].value = '\0';

	return buffer;
}


void lexer_init(struct Lexer *lexer, const char *filepath)
{
	assert(lexer != NULL && "error: NULL pointer passed to lexer_init()");
	
	lexer->filepath = filepath;
	lexer->data = read_file(filepath);
	lexer->index = 0;
	lexer->current = lexer->data[0];
}


static inline void advance(struct Lexer *lexer)
{
	lexer->index++;
	lexer->current = lexer->data[lexer->index];
}


static inline void skip_white(struct Lexer *lexer)
{
	while (isspace(lexer->current.value))
		advance(lexer);
}


static struct Token *prepare_tok_for_ret(struct Lexer *lexer, struct Token *tok)
{
	advance(lexer);

	tok->filepath = lexer->current.filepath;
	tok->line = lexer->current.line;
	tok->col = lexer->current.col;

	return tok;
}


static struct Token *parse_special(struct Lexer *lexer)
{
	switch (lexer->current.value) {
		case '(': return prepare_tok_for_ret(lexer, create_token(TOKEN_LPAREN, "("));
		case ')': return prepare_tok_for_ret(lexer, create_token(TOKEN_RPAREN, ")"));
		case '{': return prepare_tok_for_ret(lexer, create_token(TOKEN_LCURL, "{"));
		case '}': return prepare_tok_for_ret(lexer, create_token(TOKEN_RCURL, "}"));
		case ';': return prepare_tok_for_ret(lexer, create_token(TOKEN_SEMICOLON, ";"));

		case '\0': return NULL;

		default: {
			apo_compiler_err(
					lexer->current.filepath,
					lexer->current.line,
					lexer->current.col,
					"unkown character '%c'",
					lexer->current.value
			);
		}
	}

	return NULL;
}


static struct Token *parse_word(struct Lexer *lexer)
{
	char *word = calloc(1, sizeof(char));

	assert(word && "error: apollo compiler could not allocate enough memory");

	size_t line = lexer->current.line;
	size_t col = lexer->current.col;

	while (isalpha(lexer->current.value)) {
		word = realloc(word, (strlen(word) + 2) * sizeof(char));

		/* Add the current character to the word */
		strcat(word, (char []) {lexer->current.value, '\0'});
		advance(lexer);
	}


	struct Token *tok = create_token(TOKEN_WORD, word);

	tok->filepath = lexer->current.filepath;
	tok->line = line;
	tok->col = col;

	return tok;
}


static struct Token *parse_str(struct Lexer *lexer)
{
	char *str = calloc(1, sizeof(char));

	assert(str && "error: apollo compiler could not allocate enough memory");

	/* To jump over the first '"' */
	advance(lexer);

	size_t line = lexer->current.line;
	size_t col = lexer->current.col;

	while (lexer->current.value != '"') {
		str = realloc(str, (strlen(str) + 2) * sizeof(char));

		/* Add the current character to the string literal */
		strcat(str, (char []) {lexer->current.value, '\0'});
		advance(lexer);
	}


	/* To jump over the last '"' */
	advance(lexer);
	
	struct Token *tok = create_token(TOKEN_STR, str);

	tok->filepath = lexer->current.filepath;
	tok->line = line;
	tok->col = col;

	return tok;

}


static struct Token *parse_char(struct Lexer *lexer)
{
	/* To jump over the first ' */
	advance(lexer);

	char *chr = calloc(2, sizeof(char));
	*chr = lexer->current.value;

	struct Token *tok = create_token(TOKEN_CHAR, chr);

	tok->filepath = lexer->current.filepath;
	tok->line = lexer->current.line;
	tok->col = lexer->current.col;

	advance(lexer);

	if (lexer->current.value != '\'')
		apo_compiler_err(tok->filepath, tok->line, tok->col, "char literal has multiple characters");

	/* To jump over the last ' */
	advance(lexer);

	return tok;
}


static struct Token *parse_number(struct Lexer *lexer)
{
	char *number = calloc(1, sizeof(char));
	
	assert(number && "error: apollo compiler could not allocate enough memory");

	size_t line = lexer->current.line;
	size_t col = lexer->current.col;


	while (isalnum(lexer->current.value)) {
		number = realloc(number, (strlen(number) + 2) * sizeof(char));

		/* Add the current character to the number literal */
		strcat(number, (char []) {lexer->current.value, '\0'});
		advance(lexer);
	}


	struct Token *tok = create_token(TOKEN_INT, number);

	tok->filepath = lexer->current.filepath;
	tok->line = line;
	tok->col = col;

	return tok;

}


struct Token *lexer_get_token(struct Lexer *lexer)
{
	if (lexer->current.value != '\0') {
		skip_white(lexer);
	
		/* Parse a full word */
		if (isalpha(lexer->current.value))
			return parse_word(lexer);


		/* Parse a number literal */
		if (isalnum(lexer->current.value))
			return parse_number(lexer);


		/* Parse a string literal */
		if (lexer->current.value == '"')
			return parse_str(lexer);

		/* Parse a char literal */
		if (lexer->current.value == '\'')
			return parse_char(lexer);


		/* Parse a char like '{', '}', ... */
		return parse_special(lexer);
	}

	return NULL;
}


// TODO: use a linked list instead of a normal array to store the tokens
