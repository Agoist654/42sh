#include "exec1.h"

#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../builtin/builtin.h"

int ast_list_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_LIST);
        int ret = ast->ast_union.ast_list.current->ftable->exec(ast->ast_union.ast_list.current);
        if (ast->ast_union.ast_list.next != NULL)
        {
            ret = ast_list_exec(ast->ast_union.ast_list.next);
        }
        return ret;
    }
    return 1;
}

int ast_and_or_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_AND_OR);
        return ast->ast_union.ast_and_or.pipeline->ftable->exec(ast->ast_union.ast_and_or.pipeline);
    }
    return 1;
}

int ast_pipeline_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_PIPELINE);
        return ast->ast_union.ast_pipeline.command->ftable->exec(ast->ast_union.ast_pipeline.command);
    }
    return 1;
}

int ast_command_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMMAND);
        return ast->ast_union.ast_command.first->ftable->exec(ast->ast_union.ast_command.first);
    }
    return 1;
}

int ast_simple_command_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SIMPLE_COMMAND);
        // need to call expansion
        if (strcmp(ast->ast_union.ast_simple_command.argv[0], "echo") == 0)
        {
            echo(ast->ast_union.ast_simple_command.argv);
            return 0;
        }
        else if (strcmp(ast->ast_union.ast_simple_command.argv[0], "true") == 0)
        {
            return true_f();
        }
        else if (strcmp(ast->ast_union.ast_simple_command.argv[0], "false") == 0)
        {
            return false_f();
        }
        else
        {
            int pid = fork();
            if (pid == 0)
            {
                execvp(ast->ast_union.ast_simple_command.argv[0], ast->ast_union.ast_simple_command.argv);
                err(1, "failed");
            }
            else
            {
                int ret = 0;
                waitpid(pid, &ret, 0);
                return ret;
            }
        }
    }
    return 1;
}

int ast_shell_command_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SHELL_COMMAND);
        return ast->ast_union.ast_shell_command.rule_if->ftable->exec(ast->ast_union.ast_shell_command.rule_if);
    }
    return 1;
}

int ast_rule_if_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_IF);
        if (ast->ast_union.ast_rule_if.cond->ftable->exec(ast->ast_union.ast_rule_if.cond) == 0)
        {
            return ast->ast_union.ast_rule_if.then->ftable->exec(ast->ast_union.ast_rule_if.then);
        }
        else if (ast->ast_union.ast_rule_if.else_clause != NULL)
        {
            return ast->ast_union.ast_rule_if.else_clause->ftable->exec(ast->ast_union.ast_rule_if.else_clause);
        }
        return 0; //?
    }
    return 1;
}

int ast_else_clause_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_ELSE_CLAUSE);
        if (ast->ast_union.ast_else_clause.then == NULL)
        {
            return ast->ast_union.ast_else_clause.cond->ftable->exec(ast->ast_union.ast_else_clause.cond);
        }
        else
        {
            if (ast->ast_union.ast_else_clause.cond->ftable->exec(ast->ast_union.ast_else_clause.cond) == 0)
            {
                return ast->ast_union.ast_else_clause.then->ftable->exec(ast->ast_union.ast_else_clause.then);
            }
            else if (ast->ast_union.ast_else_clause.else_clause != NULL)
            {
                return ast->ast_union.ast_else_clause.else_clause->ftable->exec(ast->ast_union.ast_else_clause.else_clause);
            }
            return 0; //?
        }
    }
    return 1;
}

int ast_compound_list_exec(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMPOUND_LIST);
        int ret = ast->ast_union.ast_compound_list.and_or->ftable->exec(ast->ast_union.ast_compound_list.and_or);
        if (ast->ast_union.ast_compound_list.next != NULL)
        {
            ret = ast_compound_list_exec(ast->ast_union.ast_compound_list.next);
        }
        return ret;
    }
    return 1;
}
