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
        free(ast);
    }
    return;
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

// static void ast_rule_for_destroy(struct ast *ast)
//{
//     if (ast != NULL)
//     {
//         assert(ast->type == AST_RULE_FOR);
//         if (ast->ast_union.ast_rule_for.count != NULL)
//         {
//             ast->ast_union.ast_rule_for.count->ftable->destroy(ast->ast_union.ast_rule_for.count);
//         }
//         if (ast->ast_union.ast_rule_for.then != NULL)
//         {
//             ast->ast_union.ast_rule_for.then->ftable->destroy(ast->ast_union.ast_rule_for.then);
//         }
//         free(ast);
//     }
//     return;
// }

void ast_list_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_LIST);
        if (ast->ast_union.ast_list.current != NULL)
        {
            ast->ast_union.ast_list.current->ftable->print(
                ast->ast_union.ast_list.current);
        }
        if (ast->ast_union.ast_list.next != NULL)
        {
            printf(" ; ");
            ast->ast_union.ast_list.next->ftable->print(
                ast->ast_union.ast_list.next);
        }
    }
    printf(";\n");
    return;
}

static void ast_and_or_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_AND_OR);
        if (ast->ast_union.ast_and_or.pipeline != NULL)
        {
            ast->ast_union.ast_and_or.pipeline->ftable->print(
                ast->ast_union.ast_and_or.pipeline);
        }
    }
    return;
}

static void ast_pipeline_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_PIPELINE);
        if (ast->ast_union.ast_pipeline.command != NULL)
        {
            ast->ast_union.ast_pipeline.command->ftable->print(
                ast->ast_union.ast_pipeline.command);
        }
    }
    return;
}

static void ast_command_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_COMMAND);
        if (ast->ast_union.ast_command.first != NULL)
        {
            ast->ast_union.ast_command.first->ftable->print(
                ast->ast_union.ast_command.first);
        }
    }
    return;
}

static void ast_simple_command_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SIMPLE_COMMAND);
        if (ast->ast_union.ast_simple_command.argv != NULL)
        {
            for (int i = 0; ast->ast_union.ast_simple_command.argv[i] != NULL;
                 i++)
            {
                printf(" %s ", ast->ast_union.ast_simple_command.argv[i]);
            }
        }
    }
    return;
}

static void ast_shell_command_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_SHELL_COMMAND);
        if (ast->ast_union.ast_shell_command.rule_if != NULL)
        {
            ast->ast_union.ast_shell_command.rule_if->ftable->print(
                ast->ast_union.ast_shell_command.rule_if);
        }
    }
    return;
}

static void ast_rule_if_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_IF);
        printf(" IF ");
        if (ast->ast_union.ast_rule_if.cond != NULL)
        {
            ast->ast_union.ast_rule_if.cond->ftable->print(
                ast->ast_union.ast_rule_if.cond);
        }
        printf(" THEN ");
        if (ast->ast_union.ast_rule_if.then != NULL)
        {
            ast->ast_union.ast_rule_if.then->ftable->print(
                ast->ast_union.ast_rule_if.then);
        }
        if (ast->ast_union.ast_rule_if.else_clause != NULL)
        {
            printf("ELSE");
            ast->ast_union.ast_rule_if.else_clause->ftable->print(
                ast->ast_union.ast_rule_if.else_clause);
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
            if (ast->ast_union.ast_else_clause.cond != NULL)
            {
                ast->ast_union.ast_else_clause.cond->ftable->print(
                    ast->ast_union.ast_else_clause.cond);
            }
        }
        else
        {
            printf(" ELIF ");
            if (ast->ast_union.ast_else_clause.cond != NULL)
            {
                ast->ast_union.ast_else_clause.cond->ftable->print(
                    ast->ast_union.ast_else_clause.cond);
            }
            printf(" THEN ");
            ast->ast_union.ast_else_clause.then->ftable->print(
                ast->ast_union.ast_else_clause.then);
            if (ast->ast_union.ast_else_clause.else_clause != NULL)
            {
                printf(" ELSE ");
                ast->ast_union.ast_else_clause.else_clause->ftable->print(
                    ast->ast_union.ast_else_clause.else_clause);
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
        if (ast->ast_union.ast_compound_list.and_or != NULL)
        {
            ast->ast_union.ast_compound_list.and_or->ftable->print(
                ast->ast_union.ast_compound_list.and_or);
        }
        if (ast->ast_union.ast_compound_list.next != NULL)
        {
            printf(" ; ");
            ast_compound_list_print(ast->ast_union.ast_compound_list.next);
        }
    }
    return;
}

static void ast_rule_while_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_WHILE);
        printf(" WHILE ");
        if (ast->ast_union.ast_rule_while.cond != NULL)
        {
            ast->ast_union.ast_rule_while.cond->ftable->print(
                ast->ast_union.ast_rule_while.cond);
        }
        printf(" DO ");
        if (ast->ast_union.ast_rule_while.then != NULL)
        {
            ast->ast_union.ast_rule_while.then->ftable->print(
                ast->ast_union.ast_rule_while.then);
        }
        printf(" DONE ");
    }
    return;
}

static void ast_rule_until_print(struct ast *ast)
{
    if (ast != NULL)
    {
        assert(ast->type == AST_RULE_UNTIL);
        printf(" UNTIL ");
        if (ast->ast_union.ast_rule_until.cond != NULL)
        {
            ast->ast_union.ast_rule_until.cond->ftable->print(
                ast->ast_union.ast_rule_until.cond);
        }
        printf(" DO ");
        if (ast->ast_union.ast_rule_until.then != NULL)
        {
            ast->ast_union.ast_rule_until.then->ftable->print(
                ast->ast_union.ast_rule_until.then);
        }
        printf(" DONE ");
    }
    return;
}

// static void ast_rule_for_print(struct ast *ast)
//{
//     if (ast != NULL)
//     {
//         assert(ast->type == AST_RULE_IF);
//         printf(" FOR ");
//         if (ast->ast_union.ast_rule_for.count != NULL)
//        {
//             ast->ast_union.ast_rule_for.count->ftable->print(ast->ast_union.ast_rule_for.count);
//         }
//         printf(" DO ");
//         if (ast->ast_union.ast_rule_for.then != NULL)
//         {
//             ast->ast_union.ast_rule_for.then->ftable->print(ast->ast_union.ast_rule_for.then);
//         }
//     }
//     return;
// }

static struct ftable ftable[] = {
    { .destroy = &ast_list_destroy,
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
      .exec = &ast_compound_list_exec },
    { .destroy = &ast_rule_while_destroy,
      .print = &ast_rule_while_print,
      .exec = &ast_rule_while_exec },
    { .destroy = &ast_rule_until_destroy,
      .print = &ast_rule_until_print,
      .exec = &ast_rule_until_exec }
    //                                  { .destroy = &ast_rule_until_destroy,
    //                                    .print = &ast_rule_until_print,
    //                                    .exec = &ast_rule_until_exec }
};

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
