#define _XOPEN_SOURCE 500
#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin/builtin.h"
#include "exec.h"
#include "exec/hash_map.h"
#include "exec/hash_map_fun.h"
#include "expansion/expansion.h"

int ast_rule_while_exec(struct ast *ast, char **farg)
{
    int ret_val = 0;
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_WHILE);
    struct error *error = get_err();
    error->depth += 1;
    while (ast->ast_union.ast_rule_while.cond->ftable->exec(
               ast->ast_union.ast_rule_while.cond, farg)
               == 0
           && !error->e)
    {
        ret_val = ast->ast_union.ast_rule_while.then->ftable->exec(
            ast->ast_union.ast_rule_while.then, farg);

        if (error->c)
        {
            error->c--;
            continue;
        }
        if (error->b)
        {
            error->b--;
            break;
        }
    }
    error->depth -= 1;
    return ret_val;
}

int ast_rule_until_exec(struct ast *ast, char **farg)
{
    int ret_val = 0;
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_UNTIL);
    struct error *error = get_err();
    error->depth++;
    while (ast->ast_union.ast_rule_until.cond->ftable->exec(
               ast->ast_union.ast_rule_until.cond, farg)
               != 0
           && !error->e)
    {
        ret_val = ast->ast_union.ast_rule_until.then->ftable->exec(
            ast->ast_union.ast_rule_until.then, farg);
        if (error->c)
        {
            error->c--;
            continue;
        }
        if (error->b)
        {
            error->b--;
            break;
        }
    }
    error->depth--;
    return ret_val;
}

static int isassignment_word(char *buffer)
{
    if (buffer && isdigit(buffer[0]))
        return 0;
    for (size_t k = 0; k < strlen(buffer); k++)
    {
        if (buffer[k] != '_' && !isalnum(buffer[k]))
            return 0;
    }
    return 1;
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

int ast_rule_for_exec(struct ast *ast, char **farg)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_FOR);

    if (isassignment_word(ast->ast_union.ast_rule_for.var) == 0)
    {
        fprintf(stderr, "Bad for loop variable");
        return 2;
    }

    if (get_len(ast->ast_union.ast_rule_for.argv) == 0)
        return 0;

    int res = 0;
    struct error *error = get_err();
    error->depth++;

    // char **expanded_argv = pre_expand(ast->ast_union.ast_rule_for.argv);
    for (int k = 0; ast->ast_union.ast_rule_for.argv[k] != NULL; k++)
    {
        char *key = strdup(ast->ast_union.ast_rule_for.var);
        char *hash_key = strdup(key);
        char *value =
            expansion(strdup(ast->ast_union.ast_rule_for.argv[k]), farg);
        char *hash_value = strdup(value);
        hash_map_insert(get_hm(), hash_key, hash_value, NULL);
        res = ast_compound_list_exec(ast->ast_union.ast_rule_for.compound_list,
                                     farg);
        free(value);
        free(key);
        if (error->c)
        {
            error->c--;
            continue;
        }
        if (error->b)
        {
            error->b--;
            break;
        }
    }
    error->depth--;

    return res;
}

int ast_fundec_exec(struct ast *ast, char **farg)
{
    (void)farg;
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_FUNDEC);
    hash_map_fun_insert(get_fun_hm(), ast->ast_union.ast_fundec.name,
                        ast->ast_union.ast_fundec.body, NULL);
    ast->ast_union.ast_fundec.has_been_init = 1;
    return 0;
}
