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

static struct token token_eof = {
    .type = TOKEN_EOF,
    .buffer = NULL,
    .len = BUFFER_SIZE
};


static struct token token_error= {
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

static int check_token(enum token_type type)
{
    if (type == TOKEN_WORD || type == TOKEN_EOF || type == TOKEN_ERROR)
        return 0;
    return 1;

}

static struct lexer *lexer_init(void)
{
    struct lexer *res = malloc(sizeof(struct lexer));
    if (!res)
        return NULL;
    res->current_token = token_init();
    return res;
}

static struct token lex(void)
{
    struct token res = token_init();
    while (io_peek() != EOF)
    {
        for (enum token_type type = TOKEN_IF; type < NB_TOKENS; type++)
        {
            if (check_token(type) && strcmp(tokens[type], res.buffer) == 0)
            {
                res.type = type;
                io_pop();
                return res;
            }
        }

        if (isspace(io_peek()))
        {
            res.type = TOKEN_WORD;
            io_pop();
            break;
        }

        if (strlen(res.buffer) == res.len)
        {
            //realloc
        }
        res.buffer[strlen(res.buffer)] = io_pop();
    }
    if (io_peek() == EOF)
        res = token_eof;
    return res;
}

struct token lexer_peek(struct lexer *lexer)
{
    if (lexer->current_token.type == TOKEN_NULL)
        lex();
    return lexer->current_token;
}

struct token lexer_pop(struct lexer *lexer)
{
    char *tofree = lexer->current_token.buffer;
    lexer->current_token = lex();
    free(tofree);
    return lexer->current_token;
}

static void print_token(struct token token)
{
    for (enum token_type type = TOKEN_IF; type < NB_TOKENS; type++)
    {
        if (token.type == type)
        {
            if (token.type == TOKEN_EOF)
                printf("EOF\n");
            printf("buffer >%s<\n", token.buffer);
            printf("type >%d<\n", type);
        }
    }
}

int main(int argc, char *argv[])
{
    io_backend(argc, argv);
    struct lexer *lexer = lexer_init();
    if (!lexer)
        return -1;
    struct token token = lexer_pop(lexer);
    while (token.type != TOKEN_EOF)
    {
        print_token(token);
        token = lexer_pop(lexer);
    }
    free(token.buffer);
    //free(lexer->current_token.buffer);
    free(lexer);
    return 0;
}
