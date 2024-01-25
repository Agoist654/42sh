#define _POSIX_C_SOURCE 200112L
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin/builtin.h"
#include "dlist.h"
#include "exec.h"
#include "expansion/expansion.h"
#include "hash_map.h"
#include "pipeline.h"
#include "redirection.h"

#define NB_BUILTINS 3
static struct builtin builtins[] = {
    { .command_name = "echo", .builtin = echo },
    { .command_name = "true", .builtin = true_f },
    { .command_name = "false", .builtin = false_f }
};

int ast_list_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_LIST);
    int ret = ast->ast_union.ast_list.current->ftable->exec(
        ast->ast_union.ast_list.current);
    if (ast->ast_union.ast_list.next != NULL)
    {
        ret = ast_list_exec(ast->ast_union.ast_list.next);
    }
    return ret;
}

int ast_and_or_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_AND_OR);
    int res = ast->ast_union.ast_and_or.pipeline->ftable->exec(
        ast->ast_union.ast_and_or.pipeline);
    if (ast->ast_union.ast_and_or.next != NULL)
    {
        if (ast->ast_union.ast_and_or.and_or == AND && res == 0)
        {
            res = ast_and_or_exec(ast->ast_union.ast_and_or.next);
        }
        if (ast->ast_union.ast_and_or.and_or == OR && res != 0)
        {
            res = ast_and_or_exec(ast->ast_union.ast_and_or.next);
        }
    }
    return res;
}

int ast_pipeline_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_PIPELINE);
    int ret_value = 0;
    if (ast->ast_union.ast_pipeline.next == NULL)
        ret_value = ast->ast_union.ast_pipeline.command->ftable->exec(
            ast->ast_union.ast_pipeline.command);
    if (ast->ast_union.ast_pipeline.next != NULL)
    {
        ret_value = exec_pipe(ast);
    }
    if (ast->ast_union.ast_pipeline.neg)
        return !ret_value;
    return ret_value;
}

int ast_command_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_COMMAND);
    //    int to_close = 0;
    struct dlist *dlist = dlist_init();
    if (ast->ast_union.ast_command.redirection != NULL)
    {
        //      to_close = 1;
        for (int i = 0; ast->ast_union.ast_command.redirection[i] != NULL; i++)
        {
            exec_redirection(dlist, ast->ast_union.ast_command.redirection[i]);
        }
    }
    int res = ast->ast_union.ast_command.first->ftable->exec(
        ast->ast_union.ast_command.first);
    restore_redirection(dlist);
    return res;
}

static char *get_value(char *buffer)
{
    return strchr(buffer, '=') + 1;
}

static size_t get_equal_index(char *buffer)
{
    size_t index = 0;
    for (; index < strlen(buffer); index++)
    {
        if (buffer[index] == '=')
            break;
    }
    return index;
}

static char *get_key(char *buffer)
{
    size_t equal_index = get_equal_index(buffer);
    buffer[equal_index] = '\0';
    return buffer;
}

static size_t get_len(char **argv)
{
    if (!argv)
        return 0;
    size_t res = 0;
    while (argv[res] != NULL)
        res++;
    return res;
}

static int hash_map_add(struct ast *ast)
{
    for (size_t k = 0; k < get_len(ast->ast_union.ast_simple_command.ass_word); k++)
    {
        char *value = get_value(ast->ast_union.ast_simple_command.ass_word[k]);
        char *key = get_key(ast->ast_union.ast_simple_command.ass_word[k]);
        hash_map_insert(get_hm(), key, value, NULL);
    }
    return 1;
}

static int sent_env_add(struct ast *ast)
{
    for (size_t k = 0; k < get_len(ast->ast_union.ast_simple_command.ass_word);
         k++)
    {
        char *value = get_value(ast->ast_union.ast_simple_command.ass_word[k]);
        char *key = get_key(ast->ast_union.ast_simple_command.ass_word[k]);
        setenv(key, value, 1);
    }
    return 1;
}

int ast_simple_command_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_SIMPLE_COMMAND);
    int res = 0;
    if (get_len(ast->ast_union.ast_simple_command.argv) == 0)
    {
        hash_map_add(ast);
        return 1;
    }
    else
        sent_env_add(ast);

    struct dlist *dlist = dlist_init();
    if (ast->ast_union.ast_simple_command.redirection != NULL
        && ast->ast_union.ast_simple_command.redirection[0] != NULL)
    {
        for (int i = 0;
             ast->ast_union.ast_simple_command.redirection[i] != NULL; i++)
        {
            exec_redirection(dlist,
                             ast->ast_union.ast_simple_command.redirection[i]);
        }
    }
    for (int i = 0; ast->ast_union.ast_simple_command.argv[i] != NULL; i++)
    {
        ast->ast_union.ast_simple_command.argv[i] =
            expansion(ast->ast_union.ast_simple_command.argv[i]);
    }

    for (int k = 0;
         /*get_len(ast->ast_union.ast_simple_command.argv) != 0 && */ k
         < NB_BUILTINS;
         k++)
    {
        if (strcmp(ast->ast_union.ast_simple_command.argv[0],
                   builtins[k].command_name)
            == 0)
        {
            res = builtins[k].builtin(ast->ast_union.ast_simple_command.argv);
            restore_redirection(dlist);
            return res;
        }
    }

    /* if it is not a builtin */
    int pid = fork();
    if (pid == 0)
    {
        if (execvp(ast->ast_union.ast_simple_command.argv[0],
                   ast->ast_union.ast_simple_command.argv)
            == -1)
        {
            if (errno == ENOENT)
                errx(127, "commandd not found");
        }
        errx(1, "failedd");
    }
    waitpid(pid, &res, 0);
    restore_redirection(dlist);
    return WEXITSTATUS(res);
}

int ast_shell_command_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_SHELL_COMMAND);
    return ast->ast_union.ast_shell_command.rule_if->ftable->exec(
        ast->ast_union.ast_shell_command.rule_if);
}

int ast_rule_if_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_IF);
    if (ast->ast_union.ast_rule_if.cond->ftable->exec(
            ast->ast_union.ast_rule_if.cond)
        == 0)
    {
        return ast->ast_union.ast_rule_if.then->ftable->exec(
            ast->ast_union.ast_rule_if.then);
    }
    else if (ast->ast_union.ast_rule_if.else_clause != NULL)
    {
        return ast->ast_union.ast_rule_if.else_clause->ftable->exec(
            ast->ast_union.ast_rule_if.else_clause);
    }
    return 0; //?
}

int ast_else_clause_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_ELSE_CLAUSE);
    if (ast->ast_union.ast_else_clause.then == NULL)
    {
        return ast->ast_union.ast_else_clause.cond->ftable->exec(
            ast->ast_union.ast_else_clause.cond);
    }
    else
    {
        if (ast->ast_union.ast_else_clause.cond->ftable->exec(
                ast->ast_union.ast_else_clause.cond)
            == 0)
        {
            return ast->ast_union.ast_else_clause.then->ftable->exec(
                ast->ast_union.ast_else_clause.then);
        }
        else if (ast->ast_union.ast_else_clause.else_clause != NULL)
        {
            return ast->ast_union.ast_else_clause.else_clause->ftable->exec(
                ast->ast_union.ast_else_clause.else_clause);
        }
        return 0; //?
    }
}

int ast_compound_list_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_COMPOUND_LIST);
    int ret = ast->ast_union.ast_compound_list.and_or->ftable->exec(
        ast->ast_union.ast_compound_list.and_or);
    if (ast->ast_union.ast_compound_list.next != NULL)
    {
        ret = ast_compound_list_exec(ast->ast_union.ast_compound_list.next);
    }
    return ret;
}
