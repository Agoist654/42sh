#ifndef LEXER_H
#define LEXER_H

#define NB_TOKENS 32

#include <stddef.h>
enum token_type
{
    //step1
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_WORD,
    TOKEN_SEMICOLON,
    TOKEN_NEWLINE,
    TOKEN_OPERATOR,
    TOKEN_SINGLE_QUOTE,
    TOKEN_COMMENT,
    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_NULL,

    //step2
    TOKEN_WHILE                  ,
    TOKEN_FOR                    ,
    TOKEN_UNTIL                  ,
    TOKEN_DO                     ,
    TOKEN_DONE                   ,
    TOKEN_IN                     ,
    TOKEN_PIPE                   ,
    TOKEN_NEGATION               ,
    TOKEN_AND                    ,
    TOKEN_OR                     ,
    TOKEN_IONUMBER               ,
    TOKEN_REDIRECTION_RIGHT      ,
    TOKEN_REDIRECTION_LEFT       ,
    TOKEN_REDIRECTION_RIGHT_RIGHT,
    TOKEN_REDIRECTION_RIGHT_AND  ,
    TOKEN_REDIRECTION_LEFT_AND   ,
    TOKEN_REDIRECTION_RIGHT_PIPE ,
    TOKEN_REDIRECTION_LEFT_RIGHT

};

struct token
{
    enum token_type type;
    char *buffer;
    size_t len;
};

struct lexer
{
    struct token current_token;
};

struct lexer *lexer_init(void);
struct token lexer_peek(struct lexer *lexer);
struct token lexer_pop(struct lexer *lexer);

#endif // LEXER_H
