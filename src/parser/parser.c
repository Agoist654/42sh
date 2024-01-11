#include "parser.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/lexer.h"
#include "../io_backend/io_backend.h"

void parse_element(struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_WORD)
    {
        lexer_pop(lexer);
    }
    else
    {
        printf("KO");
    }
}

void parse_simple_command(struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_WORD)
    {
        lexer_pop(lexer);
        while(lexer_peek(lexer).type == TOKEN_WORD)
        {
            parse_element(lexer);
        }
    }
    else
    {
        printf("KO");
    }
}

void parse_command(struct lexer *lexer)
{
    parse_simple_command(lexer);
}

void parse_pipeline(struct lexer *lexer)
{
    parse_command(lexer);
}

void parse_and_or(struct lexer *lexer)
{
    parse_pipeline(lexer);
}

void parse_list(struct lexer *lexer)
{
    parse_and_or(lexer);

}

void parse_input(struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_NEWLINE || lexer_peek(lexer).type == TOKEN_EOF)
    {
        lexer_pop(lexer);
        printf("OK");
        return;
    }
    else
    {
        parse_list(lexer);
        if (lexer_peek(lexer).type == TOKEN_NEWLINE || lexer_peek(lexer).type == TOKEN_EOF)
        {
            lexer_pop(lexer);
            printf("OK");
            return;
        }
    }
    printf("KO");
    return;
}

int main(int argc, char *argv[])
{
    io_backend(argc, argv);
    struct lexer *lexer = lexer_init();
    parse_input(lexer);
    return 0;
}
