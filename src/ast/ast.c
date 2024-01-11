#include "ast.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ast_destroy_list(struct ast *ast)
{
    free(ast);
    return ;
}

void ast_destroy_if(struct ast *ast)
{
    free(ast);
    return ;
}

void ast_destroy_simple_cmd(struct ast *ast)
{
    free(ast);
    return ;
}

void ast_print_list(struct ast *ast)
{
    return;
}

void ast_print_if(struct ast *ast)
{
    return ;
}


void ast_print_simple_cmd(struct ast *ast)
{
    return ;
}

void ast_destroy(struct ast *ast)
{
    if (ast == NULL)
        return;
    for (enum ast_type type = AST_LIST; type < NB_AST; type++)
    {
        if (type == ast->type)
            asts[type].destroy(ast);
    }
}

void ast_print(struct ast *ast)
{
    if (ast == NULL)
        return;
    for (enum ast_type type = AST_LIST; type < NB_AST; type++)
    {
        if (type == ast->type)
            asts[type].print(ast);
    }
}

struct ast *ast_init(void)
{
    struct ast *res = malloc(sizeof(struct ast));
    //for (enum ast_type type = AST_LIST; type < NB_AST; type++)
    //{
    //    if (asts[type] == ast->type)
    //        asts[type].init(ast);
    //}
    return res;
}

/*
static void ast_list_free(struct ast_list *ast)
{
    if (ast != NULL)
    {
        for (int i = 0; i < ast->nb_child; i++)
        {
            ast_free(ast->child[i]);
        }
        free(ast);
    }
}

static void ast_if_free(struct ast_if *ast)
{
    if (ast != NULL)
    {
        ast_free(ast->condition);
        ast_free(ast->then);
        for(int i = 0; i < ast->nb_elif; i++)
        {
            ast_free(ast->elif[i]);
        }
        ast_free(ast->lse);
        free(ast);
    }
}


static void ast_simple_cmd_free(struct ast_simple_cmd *ast)
{
    if (ast != NULL)
    {
        for(int i = 0; i < ast->nb_args; i++)
        {
            free(ast->args[i].//to complete);
        }
        free(ast->cmd_name.//to complete);
        free(ast);
    }
}


static void ast_cmd_list(struct ast_cmd_list *ast)
{
    if (ast != NULL)
    {
        for (int i = 0; i < ast->nb_cmd; i++)
        {
            ast_free(ast->cmd_list[i]);
        }
        free(ast);
    }
}

struct ast *get_ast(void)
{
    return input();
}

struct ast *input(void)
{
    if (lexer_peek().type == TOKEN_NEWLINE || lexer_peek().type == TOKEN_EOF)
    {
        return NULL;
    }
    return NULL;
}
*/
