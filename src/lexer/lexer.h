#ifndef LEXER_H
#define LEXER_H

#define NB_TOKENS 14

#include <stddef.h>
enum token_type
{
    TOKEN_IF          ,
    TOKEN_THEN        ,
    TOKEN_ELIF        ,
    TOKEN_ELSE        ,
    TOKEN_FI          ,
    TOKEN_WORD        ,
    TOKEN_SEMICOLON  ,
    TOKEN_NEWLINE     ,
    TOKEN_OPERATOR    ,
    TOKEN_SINGLE_QUOTE,
    TOKEN_COMMENT     ,
    TOKEN_EOF         ,
    TOKEN_ERROR       ,
    TOKEN_NULL        
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
struct token lexer_pop( struct lexer *lexer);


#endif // LEXER_H
