#define LEN 8

#include "parser.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "io_backend/io_backend.h"
#include "lexer/lexer.h"

static struct error error = { .res = 0, .msg = NULL };

static struct ast *parse_shell_command(struct lexer *lexer);

// static
// char *parse_element(struct lexer *lexer)
//{
//     //if (is_in(io_peek(), redirection)
//     //{
//     //parse_redirection();
//     //}
//     return lexer_pop(lexer).buffer;
// }

static int isseparator(struct token token)
{
    return (token.type == TOKEN_EOF || token.type == TOKEN_NEWLINE
            || token.type == TOKEN_SEMICOLON);
}

static struct ast *parse_simple_command(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_SIMPLE_COMMAND);
    if (!res)
        goto error;
    res->ast_union.ast_simple_command.argv = calloc(LEN, sizeof(char *));
    if (!res->ast_union.ast_simple_command.argv)
        goto error;
    res->ast_union.ast_simple_command.len = LEN;
    size_t nb_arg = 0;
    while (!isseparator(lexer_peek(lexer)))
    {
        if (nb_arg == res->ast_union.ast_simple_command.len - 1)
            res->ast_union.ast_simple_command.argv =
                realloc(res->ast_union.ast_simple_command.argv,
                        res->ast_union.ast_simple_command.len * 2);
        res->ast_union.ast_simple_command.argv[nb_arg++] =
            lexer_pop(lexer).buffer;
    }
    res->ast_union.ast_simple_command.argv[nb_arg] = NULL;
    return res;
error:
    error.msg = "ast_pipeline init\n";
    error.res = -42;
    return NULL;
}

static struct ast *parse_command(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_COMMAND);
    if (!res)
        goto error;
    // if (is_in(lexer_peek(lexer), first_shell_command)
    if (lexer_peek(lexer).type == TOKEN_IF)
        res->ast_union.ast_command.first = parse_shell_command(lexer);
    // if (is_in(lexer_peek(lexer), first_simple_command)
    else
        res->ast_union.ast_command.first = parse_simple_command(lexer);
    return res;
error:
    error.msg = "ast_pipeline init\n";
    error.res = -42;
    return NULL;
}

static struct ast *parse_pipeline(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_PIPELINE);
    if (!res)
        goto error;
    res->ast_union.ast_pipeline.command = parse_command(lexer);
    return res;
error:
    error.msg = "ast_pipeline init\n";
    error.res = -42;
    return NULL;
}

static struct ast *parse_and_or(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_AND_OR);
    if (!res)
        goto error;
    res->ast_union.ast_and_or.pipeline = parse_pipeline(lexer);
    return res;
error:
    error.msg = "ast_and_or init\n";
    error.res = -42;
    return NULL;
}

// not sure where coumpound_list = and_or ';' '\n' '\n' '\n' '\n' ->bit strange
// but seems to be good
static struct ast *parse_compound_list(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_COMPOUND_LIST);
    if (!res)
    {
        error.msg = "ast_and_or init\n";
        error.res = -42;
        return NULL;
    }

    res->ast_union.ast_compound_list.next = NULL;

    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);
    res->ast_union.ast_compound_list.and_or = parse_and_or(lexer);
    if (error.res != 0)
        return res;

    if (lexer_peek(lexer).type == TOKEN_SEMICOLON
        || lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);
    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);

    // if (is_in(lexer_peek(lexer), first_and_or))
    if (lexer_peek(lexer).type == TOKEN_THEN
        || lexer_peek(lexer).type == TOKEN_FI
        || lexer_peek(lexer).type == TOKEN_ELSE
        || lexer_peek(lexer).type == TOKEN_ELIF)
        return res;
    res->ast_union.ast_compound_list.next = parse_compound_list(lexer);
    return res;
    // else
    // break;
}

static struct ast *parse_else_clause(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_ELSE_CLAUSE);
    if (!res)
        goto error;
    free(lexer_pop(lexer).buffer);
    res->ast_union.ast_rule_if.else_clause = NULL;

    res->ast_union.ast_else_clause.cond = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type != TOKEN_THEN)
        return res;
    free(lexer_pop(lexer).buffer);
    res->ast_union.ast_else_clause.then = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    // if is(else_clause)
    if (lexer_peek(lexer).type == TOKEN_ELSE
        || lexer_peek(lexer).type == TOKEN_ELIF)
        res->ast_union.ast_else_clause.else_clause = parse_else_clause(lexer);

    // no need to check error beacause return anyway
    return res;

error:
    error.msg = "ast_shell_command init\n";
    error.res = -42;
    return NULL;
}

static struct ast *parse_rule_if(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_RULE_IF);
    if (!res)
    {
        error.msg = "ast_shell_command init\n";
        error.res = -42;
        return NULL;
    }
    res->ast_union.ast_else_clause.then = NULL;
    res->ast_union.ast_else_clause.else_clause = NULL;

    free(lexer_pop(lexer).buffer);
    // if (isin compound_list)
    res->ast_union.ast_rule_if.cond = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type != TOKEN_THEN)
    {
        error.res = 2;
        error.msg = "parse_rule_if: syntax error near unexpected token then";
        return res;
    }

    else
        free(lexer_pop(lexer).buffer);
    // if isin
    res->ast_union.ast_rule_if.then = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    // if isin
    if (lexer_peek(lexer).type == TOKEN_ELSE
        || lexer_peek(lexer).type == TOKEN_ELIF)
    {
        res->ast_union.ast_rule_if.else_clause = parse_else_clause(lexer);
        if (error.res != 0)
            return res;
    }
    if (lexer_peek(lexer).type != TOKEN_FI)
    {
        error.res = 2;
        error.msg = "parse_rule_if: syntax error near unexpected token fi";
        return res;
    }
    free(lexer_pop(lexer).buffer);
    return res;
}

static struct ast *parse_rule_while(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_RULE_WHILE);
    if (!res)
    {
        error.msg = "ast_shell_command init\n";
        error.res = -42;
        return NULL;
    }

    free(lexer_pop(lexer).buffer);
    // if (isin compound_list)
    res->ast_union.ast_rule_while.cond = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type != TOKEN_DO)
    {
        error.res = 2;
        error.msg = "parse_rule_while: syntax error near unexpected token do";
        return res;
    }

    else
        free(lexer_pop(lexer).buffer);
    // if isin
    res->ast_union.ast_rule_while.then = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    // if isin
    if (lexer_peek(lexer).type != TOKEN_DONE)
    {
        error.res = 2;
        error.msg = "parse_rule_while: syntax error near unexpected token done";
        return res;
    }
    free(lexer_pop(lexer).buffer);
    return res;
}

static struct ast *parse_rule_until(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_RULE_UNTIL);
    if (!res)
    {
        error.msg = "ast_shell_command init\n";
        error.res = -42;
        return NULL;
    }

    free(lexer_pop(lexer).buffer);
    // if (isin compound_list)
    res->ast_union.ast_rule_until.cond = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type != TOKEN_DO)
    {
        error.res = 2;
        error.msg = "parse_rule_until: syntax error near unexpected token do";
        return res;
    }

    else
        free(lexer_pop(lexer).buffer);
    // if isin
    res->ast_union.ast_rule_until.then = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    // if isin
    if (lexer_peek(lexer).type != TOKEN_DONE)
    {
        error.res = 2;
        error.msg = "parse_rule_until: syntax error near unexpected token done";
        return res;
    }
    free(lexer_pop(lexer).buffer);
    return res;
}

static struct ast *parse_shell_command(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_SHELL_COMMAND);
    if (!res)
        goto error;
    res->ast_union.ast_shell_command.rule_if = parse_rule_if(lexer);
    return res;
error:
    error.msg = "ast_shell_command init\n";
    error.res = -42;
    return NULL;
}

static
struct ast *parse_list(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_LIST);

    if (!res)
        goto error;
    res->ast_union.ast_list.next = NULL;
    // if (is_in(lexer_peek(lexer), first_and_or))
    res->ast_union.ast_list.current = parse_and_or(lexer);
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type == TOKEN_SEMICOLON)
    {
        free(lexer_pop(lexer).buffer);
    }
    //isin and or
    if (lexer_peek(lexer).type != TOKEN_EOF && lexer_peek(lexer).type != TOKEN_NEWLINE)
    {
        res->ast_union.ast_list.next = parse_list(lexer);
    }
    return res;
error:
    error.msg = "ast_list init\n";
    error.res = -42;
    return NULL;
}

struct ast *parse_input(struct lexer *lexer)
{
    if (lexer_peek(lexer).type == TOKEN_NEWLINE
        || lexer_peek(lexer).type == TOKEN_EOF)
    {
        free(lexer_pop(lexer).buffer);
        return NULL;
    }

    struct ast *res = parse_list(lexer);
    if (error.res != 0)
    {
        ast_list_destroy(res);
        free(lexer_peek(lexer).buffer);
        errx(2, "parse input - bad parsing\n");
    }

    if (lexer_peek(lexer).type == TOKEN_NEWLINE
        || lexer_peek(lexer).type == TOKEN_EOF)
    {
        free(lexer_pop(lexer).buffer);
        return res;
    }
    ast_list_destroy(res);
    free(lexer_peek(lexer).buffer);
    errx(2, "wrong parse_input\n");
    return NULL;
}
