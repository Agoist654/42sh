#define _XOPEN_SOURCE 500
#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#include "builtin/builtin.h"
#include "exec.h"
#include "exec/hash_map.h"
#include "expansion/expansion.h"

int ast_rule_while_exec(struct ast *ast)
{
    int ret_val = 0;
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_WHILE);
    while (ast->ast_union.ast_rule_while.cond->ftable->exec(
               ast->ast_union.ast_rule_while.cond)
           == 0)
    {
        ret_val = ast->ast_union.ast_rule_while.then->ftable->exec(
            ast->ast_union.ast_rule_while.then);
    }
    return ret_val;
}

int ast_rule_until_exec(struct ast *ast)
{
    int ret_val = 0;
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_UNTIL);
    while (ast->ast_union.ast_rule_until.cond->ftable->exec(
               ast->ast_union.ast_rule_until.cond)
           == 0)
    {
        ret_val = ast->ast_union.ast_rule_until.then->ftable->exec(
            ast->ast_union.ast_rule_until.then);
    }
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

static 
size_t get_len(char **argv)
{
    if (!argv)
        return 0;
    size_t res = 0;
    while (argv[res] != NULL)
        res++;
    return res;
}

int ast_rule_for_exec(struct ast *ast)
{
    if (ast == NULL)
        return -1;
    assert(ast->type == AST_RULE_FOR);

    if (isassignment_word(ast->ast_union.ast_rule_for.var) == 0)
        return 1;

    if (get_len(ast->ast_union.ast_rule_for.argv) == 0)
        return 0;

    int res = 0;

    char *key = strdup(ast->ast_union.ast_rule_for.var);
    for (int k = 0; ast->ast_union.ast_rule_for.argv[k] != NULL; k++)
    {
        char *value = strdup(ast->ast_union.ast_rule_for.argv[k]);
        hash_map_insert(get_hm(), key, value, NULL);
        res = ast_compound_list_exec(ast->ast_union.ast_rule_for.compound_list);
        free(value);
    }

    return res;
}
