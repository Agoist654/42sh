#define LEN 8

#include "parser.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "lexer/lexer.h"

static int first_list[] = { TOKEN_NEGATION,
                            TOKEN_WORD,
                            TOKEN_REDIRECTION_RIGHT,
                            TOKEN_REDIRECTION_LEFT,
                            TOKEN_REDIRECTION_RIGHT_RIGHT,
                            TOKEN_REDIRECTION_RIGHT_AND,
                            TOKEN_REDIRECTION_LEFT_AND,
                            TOKEN_REDIRECTION_RIGHT_PIPE,
                            TOKEN_REDIRECTION_LEFT_RIGHT,
                            TOKEN_IONUMBER,
                            TOKEN_WHILE,
                            TOKEN_UNTIL,
                            TOKEN_IF,
                            TOKEN_FOR,
                            TOKEN_ASSIGNMENT_WORD,
                            TOKEN_BRACKET_LEFT,
                            TOKEN_PARENTHESIS_LEFT,
                            TOKEN_SINGLE_QUOTE,
                            TOKEN_DOUBLE_QUOTE,
                            -1 };

static int first_and_or[] = { TOKEN_NEGATION,
                              TOKEN_WORD,
                              TOKEN_REDIRECTION_RIGHT,
                              TOKEN_REDIRECTION_LEFT,
                              TOKEN_REDIRECTION_RIGHT_RIGHT,
                              TOKEN_REDIRECTION_RIGHT_AND,
                              TOKEN_REDIRECTION_LEFT_AND,
                              TOKEN_REDIRECTION_RIGHT_PIPE,
                              TOKEN_REDIRECTION_LEFT_RIGHT,
                              TOKEN_IONUMBER,
                              TOKEN_WHILE,
                              TOKEN_UNTIL,
                              TOKEN_IF,
                              TOKEN_FOR,
                              TOKEN_ASSIGNMENT_WORD,
                              TOKEN_BRACKET_LEFT,
                              TOKEN_PARENTHESIS_LEFT,
                              TOKEN_SINGLE_QUOTE,
                              TOKEN_DOUBLE_QUOTE,
                              -1 };

static int first_pipeline[] = { TOKEN_NEGATION,
                                TOKEN_WORD,
                                TOKEN_REDIRECTION_RIGHT,
                                TOKEN_REDIRECTION_LEFT,
                                TOKEN_REDIRECTION_RIGHT_RIGHT,
                                TOKEN_REDIRECTION_RIGHT_AND,
                                TOKEN_REDIRECTION_LEFT_AND,
                                TOKEN_REDIRECTION_RIGHT_PIPE,
                                TOKEN_REDIRECTION_LEFT_RIGHT,
                                TOKEN_IONUMBER,
                                TOKEN_WHILE,
                                TOKEN_UNTIL,
                                TOKEN_IF,
                                TOKEN_FOR,
                                TOKEN_ASSIGNMENT_WORD,
                                TOKEN_BRACKET_LEFT,
                                TOKEN_PARENTHESIS_LEFT,
                                TOKEN_SINGLE_QUOTE,
                                TOKEN_DOUBLE_QUOTE,
                                -1 };

static int first_command[] = { TOKEN_WORD,
                               TOKEN_REDIRECTION_RIGHT,
                               TOKEN_REDIRECTION_LEFT,
                               TOKEN_REDIRECTION_RIGHT_RIGHT,
                               TOKEN_REDIRECTION_RIGHT_AND,
                               TOKEN_REDIRECTION_LEFT_AND,
                               TOKEN_REDIRECTION_RIGHT_PIPE,
                               TOKEN_REDIRECTION_LEFT_RIGHT,
                               TOKEN_IONUMBER,
                               TOKEN_WHILE,
                               TOKEN_UNTIL,
                               TOKEN_IF,
                               TOKEN_ASSIGNMENT_WORD,
                               TOKEN_FOR,
                               TOKEN_BRACKET_LEFT,
                               TOKEN_PARENTHESIS_LEFT,
                               TOKEN_SINGLE_QUOTE,
                               TOKEN_DOUBLE_QUOTE,
                               -1 };

static int first_simple_command[] = { TOKEN_WORD,
                                      TOKEN_REDIRECTION_RIGHT,
                                      TOKEN_REDIRECTION_LEFT,
                                      TOKEN_REDIRECTION_RIGHT_RIGHT,
                                      TOKEN_REDIRECTION_RIGHT_AND,
                                      TOKEN_REDIRECTION_LEFT_AND,
                                      TOKEN_REDIRECTION_RIGHT_PIPE,
                                      TOKEN_REDIRECTION_LEFT_RIGHT,
                                      TOKEN_IONUMBER,
                                      TOKEN_ASSIGNMENT_WORD,
                                      TOKEN_SINGLE_QUOTE,
                                      TOKEN_DOUBLE_QUOTE,
                                      -1 };

static int first_shell_command[] = { TOKEN_WHILE,
                                     TOKEN_UNTIL,
                                     TOKEN_IF,
                                     TOKEN_FOR,

                                     TOKEN_PARENTHESIS_LEFT,
                                     TOKEN_BRACKET_LEFT,
                                     TOKEN_BRACKET_RIGHT,
                                     -1 };

static int first_redirection[] = { TOKEN_REDIRECTION_RIGHT,
                                   TOKEN_REDIRECTION_LEFT,
                                   TOKEN_REDIRECTION_RIGHT_RIGHT,
                                   TOKEN_REDIRECTION_RIGHT_AND,
                                   TOKEN_REDIRECTION_LEFT_AND,
                                   TOKEN_REDIRECTION_RIGHT_PIPE,
                                   TOKEN_REDIRECTION_LEFT_RIGHT,
                                   TOKEN_IONUMBER,
                                   -1 };

static int first_prefix[] = {
    TOKEN_REDIRECTION_RIGHT,       TOKEN_REDIRECTION_LEFT,
    TOKEN_REDIRECTION_RIGHT_RIGHT, TOKEN_REDIRECTION_RIGHT_AND,
    TOKEN_REDIRECTION_LEFT_AND,    TOKEN_REDIRECTION_RIGHT_PIPE,
    TOKEN_REDIRECTION_LEFT_RIGHT,  TOKEN_IONUMBER,
    TOKEN_ASSIGNMENT_WORD,         -1
};

static int first_compound_list[] = { TOKEN_NEGATION,
                                     TOKEN_WORD,
                                     TOKEN_REDIRECTION_RIGHT,
                                     TOKEN_REDIRECTION_LEFT,
                                     TOKEN_REDIRECTION_RIGHT_RIGHT,
                                     TOKEN_REDIRECTION_RIGHT_AND,
                                     TOKEN_REDIRECTION_LEFT_AND,
                                     TOKEN_REDIRECTION_RIGHT_PIPE,
                                     TOKEN_REDIRECTION_LEFT_RIGHT,
                                     TOKEN_IONUMBER,
                                     TOKEN_WHILE,
                                     TOKEN_UNTIL,
                                     TOKEN_IF,
                                     TOKEN_NEWLINE,
                                     TOKEN_FOR,
                                     TOKEN_BRACKET_LEFT,
                                     TOKEN_PARENTHESIS_LEFT,
                                     TOKEN_ASSIGNMENT_WORD,
                                     TOKEN_SINGLE_QUOTE,
                                     TOKEN_DOUBLE_QUOTE,
                                     -1 };

static int first_else_clause[] = { TOKEN_ELSE, TOKEN_ELIF, -1 };

static int op[] = { TOKEN_REDIRECTION_RIGHT,       TOKEN_REDIRECTION_LEFT,
                    TOKEN_REDIRECTION_RIGHT_RIGHT, TOKEN_REDIRECTION_RIGHT_AND,
                    TOKEN_REDIRECTION_LEFT_AND,    TOKEN_REDIRECTION_RIGHT_PIPE,
                    TOKEN_REDIRECTION_LEFT_RIGHT,  -1 };

static struct err error = { .res = 0, .msg = NULL };

static struct ast *parse_shell_command(struct lexer *lexer);

static int is_in(int token_type, const int *first)
{
    for (int i = 0; first[i] != -1; i++)
    {
        if (first[i] == token_type)
            return 1;
    }
    return 0;
}

static int isseparator(struct token token)
{
    return (token.type == TOKEN_EOF || token.type == TOKEN_NEWLINE
            || token.type == TOKEN_SEMICOLON || token.type == TOKEN_PIPE
            || token.type == TOKEN_AND || token.type == TOKEN_OR
            || token.type == TOKEN_PARENTHESIS_RIGHT
            || token.type == TOKEN_PARENTHESIS_LEFT
            || token.type == TOKEN_ERROR);
}

static struct redirection *redirection_init(void)
{
    struct redirection *res = malloc(sizeof(struct redirection));
    if (!res)
        return NULL;
    res->io_number = NULL;
    res->word = NULL;

    return res;
}

static struct redirection *parse_redirection(struct lexer *lexer)
{
    struct redirection *redir = redirection_init();
    if (redir == NULL)
        return NULL;
    if (lexer_peek(lexer).type == TOKEN_IONUMBER)
        redir->io_number = lexer_pop(lexer).buffer;
    if (is_in(lexer_peek(lexer).type, op))
    {
        struct token token = lexer_pop(lexer);
        redir->op = token.type;
        free(token.buffer);
    }
    else
    {
        error.res = 2;
        return redir;
    }
    if (!isseparator(lexer_peek(lexer)))
        redir->word = lexer_pop(lexer).buffer;
    else
        error.res = 2;
    return redir;
}

static struct element *parse_element(struct lexer *lexer)
{
    struct element *elt = malloc(sizeof(struct element));
    if (elt == NULL)
        return NULL;
    if (is_in(lexer_peek(lexer).type, first_redirection))
    {
        elt->type = REDIR;
        elt->element_union.redir = parse_redirection(lexer);
        if (error.res)
        {
            redirection_destroy(elt->element_union.redir);
            free(elt);
            return NULL;
        }
    }
    else
    {
        elt->type = WORD;
        elt->element_union.word = lexer_pop(lexer).buffer;
    }
    return elt;
}

static struct prefix *parse_prefix(struct lexer *lexer)
{
    struct prefix *prefix = malloc(sizeof(struct prefix));
    if (prefix == NULL)
        return NULL;
    if (is_in(lexer_peek(lexer).type, first_redirection))
    {
        prefix->type = REDIRECTION;
        prefix->prefix_union.redir = parse_redirection(lexer);
        if (error.res)
        {
            redirection_destroy(prefix->prefix_union.redir);
            free(prefix);
            return NULL;
        }
    }
    else
    {
        prefix->type = ASSIGNMENT_WORD;
        prefix->prefix_union.assignment_word = lexer_pop(lexer).buffer;
    }
    return prefix;
}

static void realloc_argv(struct ast *res, size_t nb_arg)
{
    if (nb_arg == res->ast_union.ast_simple_command.len_argv - 1)
    {
        res->ast_union.ast_simple_command.argv = realloc(
            res->ast_union.ast_simple_command.argv,
            res->ast_union.ast_simple_command.len_argv * 2 * sizeof(char *));
        res->ast_union.ast_simple_command.len_argv *= 2;
    }
    return;
}

static void realloc_redir(struct ast *res, size_t nb_redir)
{
    if (nb_redir == res->ast_union.ast_simple_command.len_redir - 1)
    {
        res->ast_union.ast_simple_command.redirection =
            realloc(res->ast_union.ast_simple_command.redirection,
                    res->ast_union.ast_simple_command.len_redir * 2
                        * sizeof(struct redirection *));
        res->ast_union.ast_simple_command.len_redir *= 2;
    }
    return;
}

static void realloc_ass_word(struct ast *res, size_t nb_ass)
{
    if (nb_ass == res->ast_union.ast_simple_command.len_ass - 1)
    {
        res->ast_union.ast_simple_command.ass_word = realloc(
            res->ast_union.ast_simple_command.ass_word,
            res->ast_union.ast_simple_command.len_ass * 2 * sizeof(char *));
        res->ast_union.ast_simple_command.len_ass *= 2;
    }
    return;
}

static struct ast *ast_simple_command_init(void)
{
    struct ast *res = ast_init(AST_SIMPLE_COMMAND);
    if (!res)
        return NULL;
    res->ast_union.ast_simple_command.argv = calloc(LEN, sizeof(char *));
    if (!res->ast_union.ast_simple_command.argv)
    {
        free(res);
        return NULL;
    }
    res->ast_union.ast_simple_command.redirection =
        calloc(LEN, sizeof(struct redirection *));
    if (!res->ast_union.ast_simple_command.redirection)
    {
        free(res->ast_union.ast_simple_command.argv);
        free(res);
        return NULL;
    }
    res->ast_union.ast_simple_command.ass_word = calloc(LEN, sizeof(char *));
    if (!res->ast_union.ast_simple_command.ass_word)
    {
        free(res->ast_union.ast_simple_command.argv);
        free(res->ast_union.ast_simple_command.redirection);
        free(res);
        return NULL;
    }
    res->ast_union.ast_simple_command.len_argv = LEN;
    res->ast_union.ast_simple_command.len_redir = LEN;
    res->ast_union.ast_simple_command.len_ass = LEN;
    return res;
}

static struct ast *parse_simple_command(struct lexer *lexer)
{
    struct ast *res = ast_simple_command_init();
    if (!res)
        goto error;
    size_t nb_arg = 0;
    size_t nb_redir = 0;
    size_t nb_ass = 0;
    while (is_in(lexer_peek(lexer).type, first_prefix))
    {
        realloc_ass_word(res, nb_ass);
        realloc_redir(res, nb_redir);
        struct prefix *prefix = parse_prefix(lexer);
        if (prefix->type == REDIRECTION)
            res->ast_union.ast_simple_command.redirection[nb_redir++] =
                prefix->prefix_union.redir;
        if (prefix->type == ASSIGNMENT_WORD)
            res->ast_union.ast_simple_command.ass_word[nb_ass++] =
                prefix->prefix_union.assignment_word;
        free(prefix);
    }
    while (!isseparator(lexer_peek(lexer)))
    {
        realloc_argv(res, nb_arg);
        realloc_redir(res, nb_redir);
        struct element *elt = parse_element(lexer);
        if (!elt)
            goto error;
        if (elt->type == WORD)
            res->ast_union.ast_simple_command.argv[nb_arg++] =
                elt->element_union.word;
        if (elt->type == REDIR)
            res->ast_union.ast_simple_command.redirection[nb_redir++] =
                elt->element_union.redir;
        free(elt);
    }
    res->ast_union.ast_simple_command.argv[nb_arg] = NULL;
    res->ast_union.ast_simple_command.redirection[nb_redir] = NULL;
    res->ast_union.ast_simple_command.ass_word[nb_ass] = NULL;
    return res;
error:
    error.msg = "ast_pipeline init\n";
    error.res = -42;
    if (res)
        return res;
    return NULL;
}

static struct ast *parse_fundec(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_FUNDEC);
    if (!res)
        goto error;
    res->ast_union.ast_fundec.name = lexer_pop(lexer).buffer;
    free(lexer_pop(lexer).buffer);
    res->ast_union.ast_fundec.body = NULL;
    if (lexer_peek(lexer).type == TOKEN_PARENTHESIS_RIGHT)
        free(lexer_pop(lexer).buffer);
    else
    {
        error.res = 2;
        return res;
    }
    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
    {
        free(lexer_pop(lexer).buffer);
    }
    if (is_in(lexer_peek(lexer).type, first_shell_command))
        res->ast_union.ast_fundec.body = parse_shell_command(lexer);
    else
        error.res = 2;
    return res;
error:
    error.res = -42;
    return NULL;
}

static void add_redir_command(struct ast *res, struct lexer *lexer)
{
    int nb_redir = 0;
    while (is_in(lexer_peek(lexer).type, first_redirection))
    {
        if (res->ast_union.ast_command.redirection == NULL)
        {
            res->ast_union.ast_command.redirection =
                malloc(LEN * sizeof(struct redirection));
            if (res->ast_union.ast_command.redirection == NULL)
            {
                error.res = -42;
                return;
            }
        }
        if (res->ast_union.ast_command.len_redir - 1 == nb_redir)
        {
            res->ast_union.ast_command.redirection =
                realloc(res->ast_union.ast_command.redirection,
                        res->ast_union.ast_command.len_redir * 2);
            res->ast_union.ast_command.len_redir *= 2;
        }
        res->ast_union.ast_command.redirection[nb_redir++] =
            parse_redirection(lexer);
        if (error.res)
        {
            res->ast_union.ast_command.redirection[nb_redir] = NULL;
            return;
        }
    }
    if (res->ast_union.ast_command.redirection != NULL)
        res->ast_union.ast_command.redirection[nb_redir] = NULL;
}

static struct ast *parse_command(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_COMMAND);
    if (!res)
        goto error;
    res->ast_union.ast_command.redirection = NULL;
    if (is_in(lexer_peek(lexer).type, first_simple_command))
    {
        if (lexer_look_ahead(lexer).type == TOKEN_PARENTHESIS_LEFT)
        {
            res->ast_union.ast_command.first = parse_fundec(lexer);
            add_redir_command(res, lexer);
        }
        else
            res->ast_union.ast_command.first = parse_simple_command(lexer);
        return res;
    }
    else if (is_in(lexer_peek(lexer).type, first_shell_command))
    {
        res->ast_union.ast_command.first = parse_shell_command(lexer);
        if (error.res)
            return res;
        add_redir_command(res, lexer);
        return res;
    }
    else
        error.res = 2;
    return res;
error:
    error.msg = "ast_pipeline init\n";
    error.res = -42;
    return NULL;
}

static struct ast *parse_pipeline_aux(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_PIPELINE);
    if (!res)
        goto error;
    res->ast_union.ast_pipeline.neg = 0;
    res->ast_union.ast_pipeline.next = NULL;
    res->ast_union.ast_pipeline.command = parse_command(lexer);
    if (error.res)
        return res;
    if (lexer_peek(lexer).type == TOKEN_PIPE)
    {
        free(lexer_pop(lexer).buffer);
        while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        {
            free(lexer_pop(lexer).buffer);
        }
        if (is_in(lexer_peek(lexer).type, first_command))
            res->ast_union.ast_pipeline.next = parse_pipeline_aux(lexer);
        else
            error.res = 2;
    }
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
    res->ast_union.ast_pipeline.neg = 0;
    res->ast_union.ast_pipeline.next = NULL;
    if (lexer_peek(lexer).type == TOKEN_NEGATION)
    {
        free(lexer_pop(lexer).buffer);
        res->ast_union.ast_pipeline.neg = 1;
    }
    res->ast_union.ast_pipeline.command = parse_command(lexer);
    if (error.res)
        return res;
    if (lexer_peek(lexer).type == TOKEN_PIPE)
    {
        free(lexer_pop(lexer).buffer);
        while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        {
            free(lexer_pop(lexer).buffer);
        }
        if (is_in(lexer_peek(lexer).type, first_command))
            res->ast_union.ast_pipeline.next = parse_pipeline_aux(lexer);
        else
            error.res = 2;
    }
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
    res->ast_union.ast_and_or.next = NULL;
    if (is_in(lexer_peek(lexer).type, first_pipeline))
        res->ast_union.ast_and_or.pipeline = parse_pipeline(lexer);
    else
        error.res = 2;
    if (error.res)
        return res;
    if (lexer_peek(lexer).type == TOKEN_AND
        || lexer_peek(lexer).type == TOKEN_OR)
    {
        res->ast_union.ast_and_or.and_or = lexer_peek(lexer).type - TOKEN_AND;
        free(lexer_pop(lexer).buffer);
        while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        {
            free(lexer_pop(lexer).buffer);
        }
        if (is_in(lexer_peek(lexer).type, first_and_or))
            res->ast_union.ast_and_or.next = parse_and_or(lexer);
        else
            error.res = 2;
    }
    return res;
error:
    error.msg = "ast_and_or init\n";
    error.res = -42;
    return NULL;
}

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
    if (is_in(lexer_peek(lexer).type, first_and_or))
        res->ast_union.ast_compound_list.and_or = parse_and_or(lexer);
    else
        error.res = 2;
    if (error.res)
        return res;

    if (lexer_peek(lexer).type == TOKEN_SEMICOLON
        || lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);
    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);

    if (is_in(lexer_peek(lexer).type, first_and_or))
        res->ast_union.ast_compound_list.next = parse_compound_list(lexer);
    return res;
}

static struct ast *parse_else_clause(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_ELSE_CLAUSE);
    if (!res)
        goto error;
    free(lexer_pop(lexer).buffer);
    res->ast_union.ast_rule_if.else_clause = NULL;
    res->ast_union.ast_rule_if.then = NULL;
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        res->ast_union.ast_else_clause.cond = parse_compound_list(lexer);
    else
        error.res = 2;
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type != TOKEN_THEN)
        return res;
    free(lexer_pop(lexer).buffer);
    res->ast_union.ast_else_clause.then = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    if (is_in(lexer_peek(lexer).type, first_else_clause))
        res->ast_union.ast_else_clause.else_clause = parse_else_clause(lexer);
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
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        res->ast_union.ast_rule_if.cond = parse_compound_list(lexer);
    else
    {
        error.res = 2;
        return res;
    }
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
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        res->ast_union.ast_rule_if.then = parse_compound_list(lexer);
    else
        error.res = 2;
    if (error.res != 0)
        return res;
    if (is_in(lexer_peek(lexer).type, first_else_clause))
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
    res->ast_union.ast_rule_while.then = NULL;
    res->ast_union.ast_rule_while.cond = NULL;

    free(lexer_pop(lexer).buffer);
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        res->ast_union.ast_rule_while.cond = parse_compound_list(lexer);
    else
        error.res = 2;
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
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        res->ast_union.ast_rule_while.then = parse_compound_list(lexer);
    else
        error.res = 2;
    if (error.res != 0)
        return res;
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
    res->ast_union.ast_rule_until.then = NULL;
    res->ast_union.ast_rule_until.cond = NULL;

    free(lexer_pop(lexer).buffer);
    if (is_in(lexer_peek(lexer).type, first_compound_list))
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
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        res->ast_union.ast_rule_until.then = parse_compound_list(lexer);
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type != TOKEN_DONE)
    {
        error.res = 2;
        error.msg = "parse_rule_until: syntax error near unexpected token done";
        return res;
    }
    free(lexer_pop(lexer).buffer);
    return res;
}

static void realloc_for_argv(struct ast *res, size_t nb_arg)
{
    if (nb_arg == res->ast_union.ast_rule_for.len_argv - 1)
    {
        res->ast_union.ast_rule_for.argv =
            realloc(res->ast_union.ast_rule_for.argv,
                    res->ast_union.ast_rule_for.len_argv * 2 * sizeof(char *));
        res->ast_union.ast_rule_for.len_argv *= 2;
    }
    return;
}

static void parse_rule_for_aux(struct ast **res, struct lexer *lexer)
{
    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);
    if (lexer_peek(lexer).type == TOKEN_DO)
        free(lexer_pop(lexer).buffer);
    else
        goto error;
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        *res = parse_compound_list(lexer);
    else
        goto error;
    if (lexer_peek(lexer).type == TOKEN_DONE)
        free(lexer_pop(lexer).buffer);
    else
        goto error;
    return;

error:
    error.res = 2;
    error.msg = "parse_rule_for: syntax error near unexpected token do or done";
    return;
}

static void ast_for_init(struct ast *res)
{
    res->ast_union.ast_rule_for.argv = calloc(LEN, sizeof(char *));
    res->ast_union.ast_rule_for.compound_list = NULL;
    res->ast_union.ast_rule_for.len_argv = LEN;
    return;
}

static struct ast *parse_rule_for(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_RULE_FOR);
    if (!res)
    {
        // error.msg = "ast_for init\n";
        // error.res = -42;
        return NULL;
    }

    ast_for_init(res);
    if (!res->ast_union.ast_rule_for.argv)
        goto error;
    free(lexer_pop(lexer).buffer);
    if (lexer_peek(lexer).type != TOKEN_WORD)
        goto error;
    res->ast_union.ast_rule_for.var = lexer_pop(lexer).buffer;
    if (lexer_peek(lexer).type == TOKEN_SEMICOLON)
    {
        free(lexer_pop(lexer).buffer);
        // res->ast_union.ast_rule_for.compound_list =
        // parse_rule_for_aux(lexer);
        parse_rule_for_aux(&res->ast_union.ast_rule_for.compound_list, lexer);
        if (!res->ast_union.ast_rule_for.compound_list)
            goto error;
        // even if error return res
        return res;
    }
    // not finished basrad
    while (lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);
    if (lexer_peek(lexer).type == TOKEN_IN)
        free(lexer_pop(lexer).buffer);
    else
        goto error;
    int nb_arg = 0;
    while (!isseparator(lexer_peek(lexer)))
    {
        realloc_for_argv(res, nb_arg);
        res->ast_union.ast_rule_for.argv[nb_arg++] = lexer_pop(lexer).buffer;
    }
    if (lexer_peek(lexer).type == TOKEN_SEMICOLON
        || lexer_peek(lexer).type == TOKEN_NEWLINE)
        free(lexer_pop(lexer).buffer);
    else
        goto error;
    // res->ast_union.ast_rule_for.compound_list = parse_rule_for_aux(lexer);
    parse_rule_for_aux(&res->ast_union.ast_rule_for.compound_list, lexer);
    if (!res->ast_union.ast_rule_for.compound_list)
        goto error;

    return res;

error:
    error.res = 2;
    // error.msg = "parse_rule_for: syntax error near unexpected token for or
    // in";
    return res;
}

static struct ast *parse_subshell(struct lexer *lexer)
{
    free(lexer_pop(lexer).buffer);
    struct ast *res = NULL;
    if (is_in(lexer_peek(lexer).type, first_compound_list))
        res = parse_compound_list(lexer);
    else
        error.res = 2;
    if (lexer_peek(lexer).type != TOKEN_PARENTHESIS_RIGHT)
        error.res = 2;
    free(lexer_pop(lexer).buffer);
    return res;
}

static struct ast *parse_shell_command(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_SHELL_COMMAND);
    res->ast_union.ast_shell_command.issubshell = 0;
    if (!res)
        goto error;
    if (lexer_peek(lexer).type == TOKEN_IF)
        res->ast_union.ast_shell_command.rule_if = parse_rule_if(lexer);
    else if (lexer_peek(lexer).type == TOKEN_WHILE)
        res->ast_union.ast_shell_command.rule_if = parse_rule_while(lexer);
    else if (lexer_peek(lexer).type == TOKEN_UNTIL)
        res->ast_union.ast_shell_command.rule_if = parse_rule_until(lexer);
    else if (lexer_peek(lexer).type == TOKEN_FOR)
        res->ast_union.ast_shell_command.rule_if = parse_rule_for(lexer);
    else if (lexer_peek(lexer).type == TOKEN_PARENTHESIS_LEFT)
    {
        res->ast_union.ast_shell_command.issubshell = 1;
        res->ast_union.ast_shell_command.rule_if = parse_subshell(lexer);
    }
    else if (lexer_peek(lexer).type == TOKEN_BRACKET_LEFT)
    {
        free(lexer_pop(lexer).buffer);
        if (is_in(lexer_peek(lexer).type, first_compound_list))
            res->ast_union.ast_shell_command.rule_if =
                parse_compound_list(lexer);
        else
        {
            error.res = 2;
            return res;
        }
        if (lexer_peek(lexer).type == TOKEN_BRACKET_RIGHT)
            free(lexer_pop(lexer).buffer);
        else
            error.res = 2;
        return res;
    }
    else
        error.res = 2;
    return res;
error:
    error.msg = "ast_shell_command init\n";
    error.res = -42;
    return NULL;
}

static struct ast *parse_list(struct lexer *lexer)
{
    struct ast *res = ast_init(AST_LIST);

    if (!res)
        goto error;
    res->ast_union.ast_list.next = NULL;
    if (is_in(lexer_peek(lexer).type, first_and_or))
        res->ast_union.ast_list.current = parse_and_or(lexer);
    if (error.res != 0)
        return res;
    if (lexer_peek(lexer).type == TOKEN_SEMICOLON)
    {
        free(lexer_pop(lexer).buffer);
    }
    if (is_in(lexer_peek(lexer).type, first_and_or))
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
    struct ast *res = NULL;
    if (is_in(lexer_peek(lexer).type, first_list))
        res = parse_list(lexer);
    else
        errx(2, "parse input\n");
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
