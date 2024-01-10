#ifndef LEXER_H
#define LEXER_H

#define NB_TOKENS 10

#include <stddef.h>
enum token_type
{
    TOKEN_IF        ,
    TOKEN_THEN      ,
    TOKEN_ELIF      ,
    TOKEN_ELSE      ,
    TOKEN_FI        ,
    TOKEN_SEMI_COLON,
    TOKEN_NEWLINE   ,
    TOKEN_QUOTE     ,
    TOKEN_WORD      ,
    TOKEN_EOF       ,
    TOKEN_ERROR     ,
    TOKEN_NULL     
};

static char *tokens[NB_TOKENS] = 
{
    [TOKEN_IF        ] = "if",
    [TOKEN_THEN      ] = "then",
    [TOKEN_ELIF      ] = "elif",
    [TOKEN_ELSE      ] = "else",
    [TOKEN_FI        ] = "fi",
    [TOKEN_SEMI_COLON] = ";",
    [TOKEN_NEWLINE   ] = "\n",
    [TOKEN_QUOTE     ] = "'"
    //[TOKEN_WORD      ] = "",
    //[TOKEN_EOF       ] = ""
    //[TOKEN_ERROR     ] = ""
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

//struct token lexer_peek(void);
//struct token lexer_pop(void);
struct token lexer_peek(struct lexer *lexer);
struct token lexer_pop( struct lexer *lexer);


#endif // LEXER_H
