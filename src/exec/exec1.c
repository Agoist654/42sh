#include <assert.h>
#include <err.h>
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
#include "redirection.h"

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
    int ret_value = ast->ast_union.ast_pipeline.command->ftable->exec(
        ast->ast_union.ast_pipeline.command);
    if (ast->ast_union.ast_pipeline.next != NULL)
    {
        ret_value = ast->ast_union.ast_pipeline.next->ftable->exec(
            ast->ast_union.ast_pipeline.next);
    }
    if (ast->ast_union.ast_pipeline.neg)
        return 1 - ret_value;
    return ret_value;
}

int ast_command_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_COMMAND);
    int to_close = 0;
    struct dlist *dlist = dlist_init();
    if (ast->ast_union.ast_command.redirection != NULL)
    {
        to_close = 1;
        for (int i = 0; ast->ast_union.ast_command.redirection[i] != NULL; i++)
        {
            exec_redirection(dlist, ast->ast_union.ast_command.redirection[i]);
        }
    }
    int res = ast->ast_union.ast_command.first->ftable->exec(
        ast->ast_union.ast_command.first);
    if (to_close)
        restore_redirection(dlist);
    else
        free(dlist);
    return res;
}

//#define NB_BUILTINS 3
// static struct bultin builtins[NB_BUILTINS] =
//{
//    { .command_name = "echo", .void_ppchar = NULL, .int_void = echo },
//    { .command_name = "true", .void_ppchar = true_f, .int_void = NULL },
//    { .command_name = "false", .void_ppchar = false_f, .int_void = NULL }
//};

int ast_simple_command_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_SIMPLE_COMMAND);
    int to_close = 0;
    int res = 0;
    struct dlist *dlist = dlist_init();
    if (ast->ast_union.ast_simple_command.redirection != NULL)
    {
        to_close = 1;
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
    // for (int k = 0; k < NB_BIULTINS; k++)
    //{
    //     if (strcmp(ast->ast_union.ast_simple_command.argv[0],
    //     builtins[k].command_name) == 0)
    //     {
    //     }
    // }

    if (strcmp(ast->ast_union.ast_simple_command.argv[0], "echo") == 0)
    {
        echo(ast->ast_union.ast_simple_command.argv);
        res = 0;
    }
    else if (strcmp(ast->ast_union.ast_simple_command.argv[0], "true") == 0)
    {
        res = true_f();
    }
    else if (strcmp(ast->ast_union.ast_simple_command.argv[0], "false") == 0)
    {
        res = false_f();
    }
    else
    {
        int pid = fork();
        if (pid == 0)
        {
            execvp(ast->ast_union.ast_simple_command.argv[0],
                   ast->ast_union.ast_simple_command.argv);
            err(1, "failedd");
        }
        else
        {
            waitpid(pid, &res, 0);
        }
    }
    if (to_close)
        restore_redirection(dlist);
    else
        free(dlist);
    return res;
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
