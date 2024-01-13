#include "ast.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../exec/exec1.h"

static void ast_list_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_LIST);
        ast_list_destroy(ast->ast_union.ast_list.next);
        ast->ast_union.ast_list.current->ftable->destroy(
            ast->ast_union.ast_list.current);
        free(ast);
    }
    return;
}

static void ast_and_or_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_AND_OR);
        ast->ast_union.ast_and_or.pipeline->ftable->destroy(
            ast->ast_union.ast_and_or.pipeline);
        free(ast);
    }
    return;
};

static void ast_pipeline_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_PIPELINE);
        ast->ast_union.ast_pipeline.command->ftable->destroy(
            ast->ast_union.ast_pipeline.command);
        free(ast);
    }
    return;
}

static void ast_command_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMMAND);
        ast->ast_union.ast_command.first->ftable->destroy(
            ast->ast_union.ast_command.first);
        free(ast);
    }
    return;
}

static void ast_simple_command_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SIMPLE_COMMAND);
        for (int i = 0; ast->ast_union.ast_simple_command.argv[i] != NULL; i++)
        {
            free(ast->ast_union.ast_simple_command.argv[i]);
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
        ast->ast_union.ast_shell_command.rule_if->ftable->destroy(
            ast->ast_union.ast_shell_command.rule_if);
        free(ast);
    }
    return;
}

static void ast_rule_if_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_IF);
        ast->ast_union.ast_rule_if.cond->ftable->destroy(
            ast->ast_union.ast_rule_if.cond);
        ast->ast_union.ast_rule_if.then->ftable->destroy(
            ast->ast_union.ast_rule_if.then);
        ast->ast_union.ast_rule_if.else_clause->ftable->destroy(
            ast->ast_union.ast_rule_if.else_clause);
        free(ast);
    }
    return;
}

static void ast_else_clause_destroy(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_ELSE_CLAUSE);
        ast->ast_union.ast_else_clause.cond->ftable->destroy(
            ast->ast_union.ast_else_clause.cond);
        ast->ast_union.ast_else_clause.then->ftable->destroy(
            ast->ast_union.ast_else_clause.then);
        ast->ast_union.ast_else_clause.else_clause->ftable->destroy(
            ast->ast_union.ast_else_clause.else_clause);
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
        ast->ast_union.ast_compound_list.and_or->ftable->destroy(
            ast->ast_union.ast_compound_list.and_or);
        free(ast);
    }
    return;
}

static void ast_list_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_LIST);
        ast->ast_union.ast_list.current->ftable->print(
            ast->ast_union.ast_list.current);
        if (ast->ast_union.ast_list.next != NULL)
        {
            printf(" ; ");
            ast->ast_union.ast_list.next->ftable->print(
                ast->ast_union.ast_list.next);
        }
    }
    return;
}

static void ast_and_or_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_AND_OR);
        ast->ast_union.ast_and_or.pipeline->ftable->print(
            ast->ast_union.ast_and_or.pipeline);
    }
    return;
}

static void ast_pipeline_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_PIPELINE);
        ast->ast_union.ast_pipeline.command->ftable->print(
            ast->ast_union.ast_pipeline.command);
    }
    return;
}

static void ast_command_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMMAND);
        ast->ast_union.ast_command.first->ftable->print(
            ast->ast_union.ast_command.first);
    }
    return;
}

static void ast_simple_command_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SIMPLE_COMMAND);
        for (int i = 0; ast->ast_union.ast_simple_command.argv[i] != NULL; i++)
        {
            printf(" %s ", ast->ast_union.ast_simple_command.argv[i]);
        }
    }
    return;
}

static void ast_shell_command_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SHELL_COMMAND);
        ast->ast_union.ast_shell_command.rule_if->ftable->print(ast->ast_union.ast_shell_command.rule_if);
    }
    return;
}

static void ast_rule_if_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_IF);
        printf(" IF ");
        ast->ast_union.ast_rule_if.cond->ftable->print(ast->ast_union.ast_rule_if.cond);
        printf(" THEN ");
        ast->ast_union.ast_rule_if.then->ftable->print(ast->ast_union.ast_rule_if.then);
        if (ast->ast_union.ast_rule_if.else_clause != NULL)
        {
            ast->ast_union.ast_rule_if.else_clause->ftable->print(ast->ast_union.ast_rule_if.else_clause);
        }
        printf(" FI ");
    }
    return;
}

static void ast_else_clause_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_ELSE_CLAUSE);
        if (ast->ast_union.ast_else_clause.then == NULL)
        {
            printf(" ELSE ");
            ast->ast_union.ast_else_clause.cond->ftable->print(ast->ast_union.ast_else_clause.cond);
        }
        else
        {
            printf(" ELIF ");
            ast->ast_union.ast_else_clause.cond->ftable->print(ast->ast_union.ast_else_clause.cond);
            printf(" THEN ");
            ast->ast_union.ast_else_clause.then->ftable->print(ast->ast_union.ast_else_clause.then);
            if (ast->ast_union.ast_else_clause.else_clause != NULL)
            {
                ast->ast_union.ast_else_clause.else_clause->ftable->print(ast->ast_union.ast_else_clause.else_clause);
            }
        }
    }
    return;
}

static void ast_compound_list_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMPOUND_LIST);
        ast->ast_union.ast_compound_list.and_or->ftable->print(ast->ast_union.ast_compound_list.and_or);
        if (ast->ast_union.ast_compound_list.next != NULL)
        {
            printf(" ; ");
            ast_compound_list_print(ast->ast_union.ast_compound_list.next);
        }
    }
    return;
}

static struct ftable ftable[] = { { .destroy = &ast_list_destroy,
                                    .print = &ast_list_print,
                                    .exec = &ast_list_exec },
                                  { .destroy = &ast_rule_if_destroy,
                                    .print = &ast_rule_if_print,
                                    .exec = &ast_rule_if_exec },
                                  { .destroy = &ast_simple_command_destroy,
                                    .print = &ast_simple_command_print,
                                    .exec = &ast_simple_command_exec },
                                  { .destroy = &ast_else_clause_destroy,
                                    .print = &ast_else_clause_print,
                                    .exec = &ast_else_clause_exec },
                                  { .destroy = &ast_shell_command_destroy,
                                    .print = &ast_shell_command_print,
                                    .exec = &ast_shell_command_exec },
                                  { .destroy = &ast_and_or_destroy,
                                    .print = &ast_and_or_print,
                                    .exec = &ast_and_or_exec },
                                  { .destroy = &ast_pipeline_destroy,
                                    .print = &ast_pipeline_print,
                                    .exec = &ast_pipeline_exec },
                                  { .destroy = &ast_command_destroy,
                                    .print = &ast_command_print,
                                    .exec = &ast_command_exec },
                                  { .destroy = &ast_compound_list_destroy,
                                    .print = &ast_compound_list_print,
                                    .exec = &ast_compound_list_exec } };

struct ast *ast_init(enum ast_type type)
{
    struct ast *ast = malloc(sizeof(struct ast));
    if (ast == NULL)
    {
        return NULL;
    }
    ast->type = type;
    ast->ftable = &ftable[type];
    return ast;
}
