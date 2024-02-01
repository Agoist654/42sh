#define _XOPEN_SOURCE 500
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
#include "hash_map_fun.h"
#include "pipeline.h"
#include "redirection.h"

#define NB_BUILTINS 8
FILE *stream;

static struct builtin builtins[] = {
    { .command_name = "export", .builtin = export_f },
    { .command_name = "unset", .builtin = unset_f },
    { .command_name = "echo", .builtin = echo },
    { .command_name = "true", .builtin = true_f },
    { .command_name = "false", .builtin = false_f },
    { .command_name = "exit", .builtin = exit_f },
    { .command_name = "continue", .builtin = continue_f },
    { .command_name = "break", .builtin = break_f }
};

int ast_list_exec(struct ast *ast, char **argv)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_LIST);
    int ret = ast->ast_union.ast_list.current->ftable->exec(
        ast->ast_union.ast_list.current, argv);
    if (ast->ast_union.ast_list.next != NULL)
    {
        ret = ast_list_exec(ast->ast_union.ast_list.next, argv);
    }
    return ret;
}

int ast_and_or_exec(struct ast *ast, char **argv)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_AND_OR);
    int res = ast->ast_union.ast_and_or.pipeline->ftable->exec(
        ast->ast_union.ast_and_or.pipeline, argv);
    struct error *error = get_err();
    if (error->e || (error->depth && (error->c || error->b)))
        return res;
    if (ast->ast_union.ast_and_or.next != NULL)
    {
        if (ast->ast_union.ast_and_or.and_or == AND && res == 0)
        {
            res = ast_and_or_exec(ast->ast_union.ast_and_or.next, argv);
            if (error->e || (error->depth && (error->c || error->b)))
                return res;
        }
        if (ast->ast_union.ast_and_or.and_or == OR && res != 0)
        {
            res = ast_and_or_exec(ast->ast_union.ast_and_or.next, argv);
            if (error->e || (error->depth && (error->c || error->b)))
                return res;
        }
    }
    return res;
}

int ast_pipeline_exec(struct ast *ast, char **argv)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_PIPELINE);
    struct error *error = get_err();
    int ret_value = 0;
    if (ast->ast_union.ast_pipeline.next == NULL)
        ret_value = ast->ast_union.ast_pipeline.command->ftable->exec(
            ast->ast_union.ast_pipeline.command, argv);
    if (ast->ast_union.ast_pipeline.next != NULL)
    {
        ret_value = exec_pipe(ast, argv);
    }
    if (ast->ast_union.ast_pipeline.neg && !error->e)
        return !ret_value;
    return ret_value;
}

int ast_command_exec(struct ast *ast, char **argv)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_COMMAND);
    struct dlist *dlist = dlist_init();
    if (ast->ast_union.ast_command.redirection != NULL)
    {
        for (int i = 0; ast->ast_union.ast_command.redirection[i] != NULL; i++)
        {
            exec_redirection(dlist, ast->ast_union.ast_command.redirection[i]);
        }
    }
    int res = ast->ast_union.ast_command.first->ftable->exec(
        ast->ast_union.ast_command.first, argv);
    restore_redirection(dlist);
    return res;
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

static char **pre_expand(char **argv, char **farg)
{
    size_t size = get_len(argv);
    char **res = calloc(size + 1, sizeof(char *));
    for (size_t k = 0; argv[k] != NULL; k++)
    {
        res[k] = strdup(argv[k]);
        res[k] = expansion(res[k], farg);
    }
    res[size] = NULL;
    return res;
}

static void post_expand(char **argv)
{
    for (size_t k = 0; argv[k] != NULL; k++)
    {
        // printf("POSR%s\n", argv[k]);
        free(argv[k]);
    }
    free(argv);
}

static
pid_t myexecvp(char **expanded_argv)
{
    /* if it is not a builtin */
    int pid = fork();
    if (pid == 0)
    {
        if (ftell(stream) >= 0)
            fclose(stream);
        if (execvp(expanded_argv[0], expanded_argv) == -1)
        {
            if (errno == ENOENT)
                errx(127, "commandd not found");
        }
        errx(1, "failedd");
    }
    return pid;
}

int ast_simple_command_exec(struct ast *ast, char **farg)
{
    if (ast == NULL)
        return -1;
    // assert(ast->type == AST_SIMPLE_COMMAND);
    int res = 0;
    if (get_len(ast->ast_union.ast_simple_command.argv) == 0)
    {
        hash_map_add(ast, farg);
        return 0;
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

    char **expanded_argv = pre_expand(ast->ast_union.ast_simple_command.argv, farg);
    struct ast *fun = hash_map_fun_get(get_fun_hm(), expanded_argv[0]);
    if (fun != NULL)
    {
        int res_tmp = fun->ftable->exec(fun, expanded_argv);
        if (res_tmp >= 0)
            res = res_tmp;
        restore_redirection(dlist);
        post_expand(expanded_argv);
        return res;
    }
    for (int k = 0; k < NB_BUILTINS; k++)
    {
        if (strcmp(ast->ast_union.ast_simple_command.argv[0],
                   builtins[k].command_name)
            == 0)
        {
            res = builtins[k].builtin(expanded_argv);
            // ast->ast_union.ast_simple_command.argv);
            restore_redirection(dlist);
            post_expand(expanded_argv);
            return res;
        }
    }

    pid_t pid = myexecvp(expanded_argv);
    waitpid(pid, &res, 0);
    post_expand(expanded_argv);

    restore_redirection(dlist);
    return WEXITSTATUS(res);
}

static
pid_t subshell_exec(struct ast *ast, char **farg)
{
    int pid = fork();
    if (pid == 0)
    {
        //if (ftell(stream) >= 0)
        //    fclose(stream);
        int res = ast_compound_list_exec(ast, farg);
//        ast->ast_union.ast_compound_list.and_or->ftable->exec(ast->ast_union.ast_compound_list.and_or);
        exit(res);
    }
    return pid;
}

int ast_shell_command_exec(struct ast *ast, char **farg)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_SHELL_COMMAND);
    int res = 0;
    if (ast->ast_union.ast_shell_command.issubshell == 1)
    {
        //struct hash_map *save_hm = hash_map_copy(get_hm());
        pid_t pid = subshell_exec(ast->ast_union.ast_shell_command.rule_if, farg);
        //printf("BEFOR FORK\n");
        //hash_map_dump(get_hm());
        waitpid(pid, &res, 0);
        //printf("AFTER FORK\n");
        //hash_map_dump(get_hm());
        return WEXITSTATUS(res);
    }
    return ast->ast_union.ast_shell_command.rule_if->ftable->exec(
            ast->ast_union.ast_shell_command.rule_if, farg);
}

int ast_rule_if_exec(struct ast *ast, char **farg)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_IF);
    struct error *error = get_err();
    if (ast->ast_union.ast_rule_if.cond->ftable->exec(
            ast->ast_union.ast_rule_if.cond, farg)
        == 0 && !(error->e || (error->depth && (error->c || error->b))))
    {
        return ast->ast_union.ast_rule_if.then->ftable->exec(
            ast->ast_union.ast_rule_if.then, farg);
    }
    else if (ast->ast_union.ast_rule_if.else_clause != NULL && !(error->e || (error->depth && (error->c || error->b))))
    {
        return ast->ast_union.ast_rule_if.else_clause->ftable->exec(
            ast->ast_union.ast_rule_if.else_clause, farg);
    }
    return 0;
}

int ast_else_clause_exec(struct ast *ast, char **farg)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_ELSE_CLAUSE);
    struct error *error = get_err();
    if (ast->ast_union.ast_else_clause.then == NULL)
    {
        return ast->ast_union.ast_else_clause.cond->ftable->exec(
            ast->ast_union.ast_else_clause.cond, farg);
    }
    else
    {
        if (ast->ast_union.ast_else_clause.cond->ftable->exec(
                ast->ast_union.ast_else_clause.cond, farg)
            == 0 && !(error->e || (error->depth && (error->c || error->b))))
        {
            return ast->ast_union.ast_else_clause.then->ftable->exec(
                ast->ast_union.ast_else_clause.then, farg);
        }
        else if (ast->ast_union.ast_else_clause.else_clause != NULL && !(error->e || (error->depth && (error->c || error->b))))
        {
            return ast->ast_union.ast_else_clause.else_clause->ftable->exec(
                ast->ast_union.ast_else_clause.else_clause, farg);
        }
        return 0;
    }
}

int ast_compound_list_exec(struct ast *ast, char **farg)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_COMPOUND_LIST);
    struct error *error = get_err();
    int ret = ast->ast_union.ast_compound_list.and_or->ftable->exec(
        ast->ast_union.ast_compound_list.and_or, farg);
    if (ast->ast_union.ast_compound_list.next != NULL && !(error->e || (error->depth && (error->c || error->b))))
    {
        ret = ast_compound_list_exec(ast->ast_union.ast_compound_list.next, farg);
    }
    return ret;
}
