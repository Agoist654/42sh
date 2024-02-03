#include "ast.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exec/exec.h"

void ast_list_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_LIST);
        ast_list_destroy(ast->ast_union.ast_list.next);
        if (ast->ast_union.ast_list.current != NULL)
        {
            ast->ast_union.ast_list.current->ftable->destroy(
                ast->ast_union.ast_list.current);
        }
        free(ast);
    }
    return;
}

static void ast_and_or_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_AND_OR);
        if (ast->ast_union.ast_and_or.pipeline != NULL)
        {
            ast->ast_union.ast_and_or.pipeline->ftable->destroy(
                ast->ast_union.ast_and_or.pipeline);
        }
        ast_and_or_destroy(ast->ast_union.ast_and_or.next);
        free(ast);
    }
    return;
}

static void ast_pipeline_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_PIPELINE);
        if (ast->ast_union.ast_pipeline.command != NULL)
        {
            ast->ast_union.ast_pipeline.command->ftable->destroy(
                ast->ast_union.ast_pipeline.command);
        }
        ast_pipeline_destroy(ast->ast_union.ast_pipeline.next);
        free(ast);
    }
    return;
}

void redirection_destroy(struct redirection *redir)
{
    if (redir != NULL)
    {
        if (redir->io_number != NULL)
            free(redir->io_number);
        if (redir->word != NULL)
            free(redir->word);
        free(redir);
    }
}

static void ast_command_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMMAND);
        if (ast->ast_union.ast_command.first != NULL)
        {
            ast->ast_union.ast_command.first->ftable->destroy(
                ast->ast_union.ast_command.first);
        }
        if (ast->ast_union.ast_command.redirection != NULL)
        {
            for (int i = 0; ast->ast_union.ast_command.redirection[i] != NULL;
                 i++)
            {
                redirection_destroy(ast->ast_union.ast_command.redirection[i]);
            }
            free(ast->ast_union.ast_command.redirection);
        }
        free(ast);
    }
    return;
}

static void ast_simple_command_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SIMPLE_COMMAND);
        if (ast->ast_union.ast_simple_command.argv != NULL)
        {
            for (int i = 0; ast->ast_union.ast_simple_command.argv[i] != NULL;
                 i++)
            {
                free(ast->ast_union.ast_simple_command.argv[i]);
            }
            free(ast->ast_union.ast_simple_command.argv);
        }
        if (ast->ast_union.ast_simple_command.redirection != NULL)
        {
            for (int i = 0;
                 ast->ast_union.ast_simple_command.redirection[i] != NULL; i++)
            {
                redirection_destroy(
                    ast->ast_union.ast_simple_command.redirection[i]);
            }
            free(ast->ast_union.ast_simple_command.redirection);
        }
        if (ast->ast_union.ast_simple_command.ass_word != NULL)
        {
            for (int i = 0;
                 ast->ast_union.ast_simple_command.ass_word[i] != NULL; i++)
            {
                free(ast->ast_union.ast_simple_command.ass_word[i]);
            }
            free(ast->ast_union.ast_simple_command.ass_word);
        }
        free(ast);
    }
    return;
}

static void ast_shell_command_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SHELL_COMMAND);
        if (ast->ast_union.ast_shell_command.rule_if != NULL)
        {
            ast->ast_union.ast_shell_command.rule_if->ftable->destroy(
                ast->ast_union.ast_shell_command.rule_if);
        }
        free(ast);
    }
    return;
}

static void ast_rule_if_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_IF);
        if (ast->ast_union.ast_rule_if.cond != NULL)
        {
            ast->ast_union.ast_rule_if.cond->ftable->destroy(
                ast->ast_union.ast_rule_if.cond);
        }
        if (ast->ast_union.ast_rule_if.then != NULL)
        {
            ast->ast_union.ast_rule_if.then->ftable->destroy(
                ast->ast_union.ast_rule_if.then);
        }
        if (ast->ast_union.ast_rule_if.else_clause != NULL)
        {
            ast->ast_union.ast_rule_if.else_clause->ftable->destroy(
                ast->ast_union.ast_rule_if.else_clause);
        }
        free(ast);
    }
    return;
}

static void ast_else_clause_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_ELSE_CLAUSE);
        if (ast->ast_union.ast_else_clause.cond != NULL)
        {
            ast->ast_union.ast_else_clause.cond->ftable->destroy(
                ast->ast_union.ast_else_clause.cond);
        }
        if (ast->ast_union.ast_else_clause.then != NULL)
        {
            ast->ast_union.ast_else_clause.then->ftable->destroy(
                ast->ast_union.ast_else_clause.then);
        }
        if (ast->ast_union.ast_else_clause.else_clause != NULL)
        {
            ast->ast_union.ast_else_clause.else_clause->ftable->destroy(
                ast->ast_union.ast_else_clause.else_clause);
        }
        free(ast);
    }
    return;
}

static void ast_compound_list_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMPOUND_LIST);
        ast_compound_list_destroy(ast->ast_union.ast_compound_list.next);
        if (ast->ast_union.ast_compound_list.and_or != NULL)
        {
            ast->ast_union.ast_compound_list.and_or->ftable->destroy(
                ast->ast_union.ast_compound_list.and_or);
        }
        free(ast);
    }
    return;
}

static void ast_rule_while_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_WHILE);
        if (ast->ast_union.ast_rule_while.cond != NULL)
        {
            ast->ast_union.ast_rule_while.cond->ftable->destroy(
                ast->ast_union.ast_rule_while.cond);
        }
        if (ast->ast_union.ast_rule_while.then != NULL)
        {
            ast->ast_union.ast_rule_while.then->ftable->destroy(
                ast->ast_union.ast_rule_while.then);
        }
        free(ast);
    }
    return;
}

static void ast_rule_until_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_UNTIL);
        if (ast->ast_union.ast_rule_until.cond != NULL)
        {
            ast->ast_union.ast_rule_until.cond->ftable->destroy(
                ast->ast_union.ast_rule_until.cond);
        }
        if (ast->ast_union.ast_rule_until.then != NULL)
        {
            ast->ast_union.ast_rule_until.then->ftable->destroy(
                ast->ast_union.ast_rule_until.then);
        }
        free(ast);
    }
    return;
}

static void ast_rule_for_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_FOR);
        if (ast->ast_union.ast_rule_for.var != NULL)
        {
            free(ast->ast_union.ast_rule_for.var);
        }
        if (ast->ast_union.ast_rule_for.argv != NULL)
        {
            for (int k = 0; ast->ast_union.ast_rule_for.argv[k] != NULL; k++)
                free(ast->ast_union.ast_rule_for.argv[k]);
            free(ast->ast_union.ast_rule_for.argv);
        }
        if (ast->ast_union.ast_rule_for.compound_list)
            ast->ast_union.ast_rule_for.compound_list->ftable->destroy(
                ast->ast_union.ast_rule_for.compound_list);
        free(ast);
    }
    return;
}

static void ast_fundec_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_FUNDEC);
        if (!ast->ast_union.ast_fundec.has_been_init)
        {
            if (ast->ast_union.ast_fundec.name != NULL)
                free(ast->ast_union.ast_fundec.name);
            if (ast->ast_union.ast_fundec.body != NULL)
                ast->ast_union.ast_fundec.body->ftable->destroy(
                    ast->ast_union.ast_fundec.body);
        }
        free(ast);
    }
    return;
}

static struct ftable ftable[] = {
    { .destroy = &ast_list_destroy, .exec = &ast_list_exec },
    { .destroy = &ast_rule_if_destroy, .exec = &ast_rule_if_exec },
    { .destroy = &ast_simple_command_destroy,
      .exec = &ast_simple_command_exec },
    { .destroy = &ast_else_clause_destroy, .exec = &ast_else_clause_exec },
    { .destroy = &ast_shell_command_destroy, .exec = &ast_shell_command_exec },
    { .destroy = &ast_and_or_destroy, .exec = &ast_and_or_exec },
    { .destroy = &ast_pipeline_destroy, .exec = &ast_pipeline_exec },
    { .destroy = &ast_command_destroy, .exec = &ast_command_exec },
    { .destroy = &ast_compound_list_destroy, .exec = &ast_compound_list_exec },
    { .destroy = &ast_rule_while_destroy, .exec = &ast_rule_while_exec },
    { .destroy = &ast_rule_until_destroy, .exec = &ast_rule_until_exec },
    { .destroy = &ast_rule_for_destroy, .exec = &ast_rule_for_exec },
    { .destroy = &ast_fundec_destroy, .exec = &ast_fundec_exec }
};

struct ast *ast_init(enum ast_type type)
{
    struct ast *ast = calloc(1, sizeof(struct ast));
    if (ast == NULL)
    {
        return NULL;
    }
    ast->type = type;
    ast->ftable = &ftable[type];
    return ast;
}
