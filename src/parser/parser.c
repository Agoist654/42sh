#include "parser.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer/lexer.h"
#include "../io_backend/io_backend.h"

static
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

static
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

static
void parse_rule_if(struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_ELSE || lexer_peek(lexer).type == TOKEN_ELIF)
    {
        parse_compound_list(lexer);
    }
    if (lexer_peek(lexer).type == TOKEN_THEN)
    {
        parse_compound_list(lexer);
        //if (is_in(lexer_peek(lexer), first_else_clause))
        //    parse_else_clause(lexer);
    }
}

static
void parse_shell_command(struct lexer *lexer)
{
    parse_rule_if(lexer);
}

static
void parse_command(struct lexer *lexer)
{
    // if (is_in(lexer_peek(lexer), first_simple_command)
    parse_simple_command(lexer);
    // if (is_in(lexer_peek(lexer), first_shell_command)
    //parse_shell_command(lexer);
}

static
void parse_pipeline(struct lexer *lexer)
{
    parse_command(lexer);
}



static
void parse_and_or(struct lexer *lexer)
{
    parse_pipeline(lexer);
}

//not sure where coumpound_list = and_or ';' '\n' '\n' '\n' '\n' ->bit strange but seems to be good
static 
void parse_compound_list(struct lexer *lexer)
{
    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
    }
    parse_and_or(lexer);
    while (lexer_peek(lexer).type == TOKEN_SEMICOLON || lexer_peek(lexer).type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        {
            lexer_pop(lexer);
        }
        //if (is_in(lexer_peek(lexer), first_and_or))
        parse_and_or(lexer);
        //else
            //break;
    }
    if (lexer_peek(lexer).type == TOKEN_SEMICOLON)
        lexer_pop(lexer);
    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
    }
}

static
void parse_list(struct lexer *lexer)
{
    parse_and_or(lexer);
    while (lexer_peek(lexer).type == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);
        //if (is_in(lexer_peek(lexer), first_and_or))
        parse_and_or(lexer);
    }
    if (lexer_peek(lexer).type == TOKEN_SEMICOLON)
        lexer_pop(lexer);
}

void parse_input(struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_NEWLINE || lexer_peek(lexer).type == TOKEN_EOF)
    {
        lexer_pop(lexer);
        //printf("OK");
        return;
    }
    parse_list(lexer);
    if (lexer_peek(lexer).type == TOKEN_NEWLINE || lexer_peek(lexer).type == TOKEN_EOF)
    {
        lexer_pop(lexer);
        //printf("OK");
        return;
    }
    //printf("KO");
    return;
}

int main(int argc, char *argv[])
{
    io_backend(argc, argv);
    struct lexer *lexer = lexer_init();
    parse_input(lexer);
    return 0;
}
