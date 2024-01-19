#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin/builtin.h"
#include "exec.h"
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
