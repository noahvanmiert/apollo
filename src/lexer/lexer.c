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

#define FILE_BUFFER_CAP  	4096
#define MEMORY_CHECK(ptr)	if (!ptr) { fprintf(stderr, "error: memory allocation failed"); exit(1); }


/*
 *	Prints a given error message, with the format: 'filepath:line:col: message'.
 *	@param filepath: The filepath of file where the error occurd.
 *	@param line: 	 The line where the error occurd.
 *	@param col:		 The col where the error occured.
*/
void apo_compiler_err(const char *filepath, size_t line, size_t col, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	printf("\033[1;31m");
	printf("%s:%zu:%zu: ", filepath, line, col);
	vprintf(fmt, args);
	printf("\033[0m\n");

	exit(EXIT_FAILURE);
}


/*
 *	Reads a file and returns an array were every character is converted into a character token (ctoken_t *).
 *	@param filepath: The filepath to open.
 *  @return:         The array of character tokens (ctoken_t *)
*/
static ctoken_t *__read_file(const char *filepath)
{
	FILE *fptr = fopen(filepath, "r");

	if (!fptr) {
		fprintf(stderr, "\033[1;31merror: could not open file '%s'\033[0m\n", filepath);
		exit(1);
	}

	ctoken_t *buffer = malloc(FILE_BUFFER_CAP * sizeof(ctoken_t));
	
	MEMORY_CHECK(buffer);

	size_t buffer_size = FILE_BUFFER_CAP;
	size_t buffer_index = 0;
	size_t line = 1, col = 1;

	while (1) {
		if (buffer_index >= buffer_size) {
			buffer_size += FILE_BUFFER_CAP;
			buffer = realloc(buffer, buffer_size * sizeof(ctoken_t));
			
			MEMORY_CHECK(buffer);
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

	buffer = realloc(buffer, (buffer_index + 1) * sizeof(ctoken_t));

	MEMORY_CHECK(buffer);

	buffer[buffer_index].value = '\0';

	return buffer;
}


/*
 *	Initializes a lexer object.
 *	@param lexer:    The lexer object.
 *	@param filepath: The filepath of the source file.
 *  @return:		 Nothing
*/
void lexer_init(lexer_t *lexer, const char *filepath)
{	
	lexer->filepath = filepath;
	lexer->data = __read_file(filepath);
	lexer->index = 0;
	lexer->current = lexer->data[0];
}


/*
 *	Advanced the lexer to the next character.
 *	@param lexer: The lexer object.
 *  @return:	  Nothing
*/
static inline void __advance(lexer_t *lexer)
{
	lexer->index++;
	lexer->current = lexer->data[lexer->index];
}


/*
 *	Advances while the current token is a space or space like character (\n, \t, ...)
 *	@param lexer: The lexer object.
*/
static inline void __skip_white(lexer_t *lexer)
{
	while (isspace(lexer->current.value))
		__advance(lexer);
}


/*
 *	Advances and copies the position of the lexer's current token to the given token.
 *	@param lexer: The lexer object.
 *	@param tok:	  A token object, this object will be returned in lexer_get_token()
 *  @return:	  The given token object (@param tok).
*/
static token_t *__prepare_tok_for_ret(lexer_t *lexer, token_t *tok)
{
	__advance(lexer);

	tok->filepath = lexer->current.filepath;
	tok->line = lexer->current.line;
	tok->col = lexer->current.col;

	return tok;
}


/*
 *	Parses a special character like '(', if the character is unkown there we be printed an error.
 *  @param lexer: The lexer object.
 *  @return:	  The token generated from the current character.
*/
static token_t *__parse_special(lexer_t *lexer)
{
	switch (lexer->current.value) {
		case '(': return __prepare_tok_for_ret(lexer, token_new(TOKEN_LPAREN, "("));
		case ')': return __prepare_tok_for_ret(lexer, token_new(TOKEN_RPAREN, ")"));
		case '{': return __prepare_tok_for_ret(lexer, token_new(TOKEN_LCURL, "{"));
		case '}': return __prepare_tok_for_ret(lexer, token_new(TOKEN_RCURL, "}"));
		case ':': return __prepare_tok_for_ret(lexer, token_new(TOKEN_COLON, ":"));
		case ';': return __prepare_tok_for_ret(lexer, token_new(TOKEN_SEMICOLON, ";"));
		case '=': return __prepare_tok_for_ret(lexer, token_new(TOKEN_EQ, "="));

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


/*
 *	Parses a single word.
 *	@param lexer: The lexer object.
 *	@return:	  The token with the generated word.
*/
static token_t *__parse_word(lexer_t *lexer)
{
	char *word = calloc(1, sizeof(char));

	MEMORY_CHECK(word);

	size_t line = lexer->current.line;
	size_t col = lexer->current.col;

	while (isalpha(lexer->current.value) || isalnum(lexer->current.value)) {
		word = realloc(word, (strlen(word) + 2) * sizeof(char));

		MEMORY_CHECK(word);
		
		/* Add the current character to the word */
		strcat(word, (char []) {lexer->current.value, '\0'});
		__advance(lexer);
	}


	token_t *tok = token_new(TOKEN_WORD, word);

	/*
		TODO:Keywords should not have type TOKEN_WORD
		but type TOKEN_KEYWORD
	*/

	tok->filepath = lexer->current.filepath;
	tok->line = line;
	tok->col = col;

	return tok;
}


/*
 *	Parse a single string literal.
 *	@param lexer: The lexer object.
 *	@return:	  The token with the generated word.
*/
static token_t *__parse_str(lexer_t *lexer)
{
	char *str = calloc(1, sizeof(char));

	MEMORY_CHECK(str);

	/* To jump over the first '"' */
	__advance(lexer);

	size_t line = lexer->current.line;
	size_t col = lexer->current.col;

	while (lexer->current.value != '"') {
		str = realloc(str, (strlen(str) + 2) * sizeof(char));

		MEMORY_CHECK(str);

		/* Add the current character to the string literal */
		strcat(str, (char []) {lexer->current.value, '\0'});
		__advance(lexer);
	}


	/* To jump over the last '"' */
	__advance(lexer);
	
	token_t *tok = token_new(TOKEN_STR, str);

	tok->filepath = lexer->current.filepath;
	tok->line = line;
	tok->col = col;

	return tok;

}


/*
 *	Parses a single character literal.
 *	@param lexer: The lexer object.
 *	@return:	  The token with the generated char.
*/
static token_t *__parse_char(lexer_t *lexer)
{
	/* To jump over the first ' */
	__advance(lexer);

	char *chr = calloc(2, sizeof(char));

	MEMORY_CHECK(chr);

	*chr = lexer->current.value;

	token_t *tok = token_new(TOKEN_CHAR, chr);

	tok->filepath = lexer->current.filepath;
	tok->line = lexer->current.line;
	tok->col = lexer->current.col;

	__advance(lexer);

	if (lexer->current.value != '\'')
		apo_compiler_err(tok->filepath, tok->line, tok->col, "char literal has multiple characters");

	/* To jump over the last ' */
	__advance(lexer);

	return tok;
}


/*
 *	Parses a signle number literal.
 *	@param lexer: The lexer object.
 *	@return: 	  The token with the generated number.
*/
static token_t *__parse_number(lexer_t *lexer)
{
	char *number = calloc(1, sizeof(char));

	MEMORY_CHECK(number);

	size_t line = lexer->current.line;
	size_t col = lexer->current.col;


	while (isalnum(lexer->current.value)) {
		number = realloc(number, (strlen(number) + 2) * sizeof(char));

		MEMORY_CHECK(number);

		/* Add the current character to the number literal */
		strcat(number, (char []) {lexer->current.value, '\0'});
		__advance(lexer);
	}


	token_t *tok = token_new(TOKEN_INT, number);

	tok->filepath = lexer->current.filepath;
	tok->line = line;
	tok->col = col;

	return tok;

}


/*
 *	Parses an returns the next token, the info about were we are in the source code is in the lexer object.
 *	@param lexer: The lexer object.
 *	@return:	  The next token.
*/
token_t *lexer_get_token(lexer_t *lexer)
{
	if (lexer->current.value != '\0') {
		__skip_white(lexer);
	
		/* Parse a full word */
		if (isalpha(lexer->current.value))
			return __parse_word(lexer);


		/* Parse a number literal */
		if (isalnum(lexer->current.value))
			return __parse_number(lexer);


		/* Parse a string literal */
		if (lexer->current.value == '"')
			return __parse_str(lexer);

		/* Parse a char literal */
		if (lexer->current.value == '\'')
			return __parse_char(lexer);


		/* Parse a char like '{', '}', ... */
		return __parse_special(lexer);
	}

	return token_new(TOKEN_END, "");
}
