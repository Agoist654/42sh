//#include "lexer/lexer.h"
//#include "io_backend/io_backend.h"
#include "lexer.h"
#include "../io_backend/io_backend.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SIZE 512

//static struct token current_token = {
//    .type = TOKEN_NULL,
//    .buffer = NULL,
//    .len = BUFFER_SIZE
//};

static 
char *tokens[NB_TOKENS] = 
{
    [TOKEN_IF               ] = "if",
    [TOKEN_THEN             ] = "then",
    [TOKEN_ELIF             ] = "elif",
    [TOKEN_ELSE             ] = "else",
    [TOKEN_FI               ] = "fi",
    [TOKEN_SEMI_COLON       ] = ";",
    [TOKEN_NEWLINE          ] = "\n"
    //[TOKEN_SINGLE_QUOTE     ] = "'",
    //[TOKEN_COMMENT          ] = "#"
    //[TOKEN_WORD           ] = "",
    //[TOKEN_EOF            ] = ""
    //[TOKEN_ERROR          ] = ""
};



//static struct token token_eof = {
//    .type = TOKEN_EOF,
//    .buffer = NULL,
//    .len = BUFFER_SIZE
//};


static 
struct token token_error= {
    .type = TOKEN_ERROR,
    .buffer = NULL,
    .len = BUFFER_SIZE
};

/*
static struct token token_null = {
    .type = TOKEN_NULL,
    .buffer = NULL,
    .len = BUFFER_SIZE
};
*/



/*
static struct token token_init_error(void)
{
    struct token res = {
    .type = TOKEN_ERROR,
    .buffer = NULL,
    .len = BUFFER_SIZE

    };
    return res;
}
*/

static 
struct token token_init(void)
{
    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    if (!buffer)
        return token_error;
    struct token res = {
        .type = TOKEN_NULL,
        .buffer = buffer,
        .len = BUFFER_SIZE
    };
    return res;
}

//static 
int check_token(enum token_type type)
{
    if (type == TOKEN_WORD || type == TOKEN_EOF || type == TOKEN_ERROR)
        return 0;
    return 1;
}

struct lexer *lexer_init(void)
{
    struct lexer *res = malloc(sizeof(struct lexer));
    if (!res)
        return NULL;
    res->current_token = token_init();
    return res;
}


static
struct token handle_comment(struct token token)
{
    token.type = TOKEN_COMMENT;
    while (io_peek() != EOF && io_peek() != '\n')
        token.buffer[strlen(token.buffer)] = io_pop();
    return token;
}

static
struct token handle_single_quote(struct token token)
{
    //append the quote in the buffer
    token.buffer[strlen(token.buffer)] = io_pop();
    token.type = TOKEN_SINGLE_QUOTE;
    while (io_peek() != EOF)
    {
        token.buffer[strlen(token.buffer)] = io_pop();
        if (io_peek() == '\'')
        {
            token.buffer[strlen(token.buffer)] = io_pop();
            return token;
        }
    }
    token.type = TOKEN_ERROR;
    return token;
}


static 
struct token token_reg(void)
{
    struct token res = token_init();
    //rule 1
    while (io_peek() != EOF)
    {
        //rule 2: if peek is part of the current operator res.buffer[strlen(res.buffer)] = io_pop();
    //  //rule 3: if peek is not part of current operator return the token (the operator)
        /*
         * if (is_part_prev_op(io_peek(), res.buffer))
         * {
         *
         * }
         */
        //rule 4: backslash/quote/double-quote and not quoted
        if (res.type != TOKEN_SINGLE_QUOTE && io_peek() == '\'')
            return handle_single_quote(res);

        if (res.type != TOKEN_SINGLE_QUOTE && io_peek() == '#')
            return handle_comment(res);

        //rule 5: sub shell
    //    //rule 6: if not quoted therfore: start of a new operator
        if (io_peek() == ';')
        {
            res.type = TOKEN_OPERATOR;
            res.buffer[strlen(res.buffer)] = io_pop();
            return res;
        }

    //    //rule 7: if not quoted : is a >blank< return token
        if (isspace(io_peek()))
        {
            io_pop();
            if (strcmp(res.buffer, "") != 0)
                return res;
        }
    //    //rule 8: if TOKEN_WORD : res.buffer[strlen(res.buffer)] = io_pop();
        if (res.type == TOKEN_WORD)
            res.buffer[strlen(res.buffer)] = io_pop();
    //    //rule 9: comment until \n : TOKEN_COMMENT, the \n is part of it

    //    //rule 10: start of a TOKEN_WORD
        res.type = TOKEN_WORD;

    }
    if (res.type != TOKEN_WORD && res.type != TOKEN_EOF)
        res.type = TOKEN_EOF;
    return res;
}


static struct token lex(void)
{
    struct token res = token_reg();
    if (res.type != TOKEN_WORD && res.type != TOKEN_OPERATOR)
        return res;
    for (enum token_type type = TOKEN_IF; type < NB_TOKENS; type++)
    {
        if (strcmp(tokens[type], res.buffer) == 0)
        {
            res.type = type;
            break;
        }
    }
    return res;
}

struct token lexer_peek(struct lexer *lexer)
{
    if (lexer->current_token.type == TOKEN_NULL)
        lexer->current_token = lex();
    return lexer->current_token;
}

struct token lexer_pop(struct lexer *lexer)
{
    char *tofree = lexer->current_token.buffer;
    lexer->current_token = lex();
    free(tofree);
    return lexer->current_token;
}

//static 
void print_token(struct token token)
{
    for (enum token_type type = TOKEN_IF; type < NB_TOKENS; type++)
    {
        if (token.type == type)
        {
            printf("buffer >%s<\n", token.buffer);
            printf("type >%s<\n", tokens[type]);
        }
    }
}

int main(int argc, char *argv[])
{
    io_backend(argc, argv);
    struct lexer *lexer = lexer_init();
    struct token token = lexer_pop(lexer);
    print_token(token);
    while (token.type != TOKEN_EOF)
    {
        token = lexer_pop(lexer);
        print_token(token);
    }
    free(token.buffer);
    //free(lexer->current_token.buffer);
    free(lexer);
    return 0;
}
