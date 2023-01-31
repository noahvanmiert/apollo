#ifndef __TOKEN_H_
#define __TOKEN_H_


enum TokenType {
    TOKEN_WORD,
    TOKEN_STR,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURL,
    TOKEN_RCURL,
    TOKEN_SEMICOLON,
};


struct Token {
    enum TokenType type;
    const char *value;

	struct Token *prev;
	struct Token *next;
};


const char *get_token_str(enum TokenType type);
struct Token *create_token(enum TokenType type, const char *value);

#endif // __TOKEN_H_
