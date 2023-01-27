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
    TOKEN_END
};


struct Token {
    enum TokenType type;
    const char *value;
};


const char *get_str_from_token(enum TokenType type);
struct Token *create_token(enum TokenType type, const char *value);

#endif // __TOKEN_H_
