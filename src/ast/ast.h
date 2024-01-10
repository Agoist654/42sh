#ifndef AST_H
#define AST_H

#include "../lexer/lexer.h"

#define NB_AST 3

enum ast_type
{
    AST_LIST,
    AST_IF,
    AST_SIMPLE_CMD
};

struct ast_list
{
    enum ast_type type;
    int nb_child;
    void **child;
};

struct ast_if
{
    enum ast_type type;
    struct ast_list *condition;
    struct ast_list *then;
    int nb_elif;
    struct ast_if **elif;
    struct ast_list *lse;
};

struct ast_simple_cmd
{
    enum ast_type type;
    struct token cmd_name;
    int nb_args;
    struct token *args;
};

struct ast_cmd_list
{
    enum ast_type type;
    int nb_cmd;
    void **cmd_list;
};

union ast_union
{
    struct ast_list ast_list;
    struct ast_if ast_if;
    struct ast_simple_cmd ast_simple_cmd;
};

struct ast;
typedef void (*destroyer) (struct ast *ast);
typedef void (*printer) (struct ast *ast);
typedef struct ast *(*initer) (void);

struct ast
{
    enum ast_type type;
    union ast_union ast_union;
    destroyer destroy;
    printer print;
    initer init;
};
void ast_destroy_list(struct ast *ast);
void ast_destroy_if(struct ast *ast);
void ast_destroy_simple_cmd(struct ast *ast);

void ast_print_list(struct ast *ast);
void ast_print_if(struct ast *ast);
void ast_print_simple_cmd(struct ast *ast);

struct ast *ast_init_list(void);
struct ast *ast_init_if(void);
struct ast *ast_init_simple_cmd(void);



static struct ast asts[NB_AST] =
{
        { .type = AST_LIST,  .destroy = ast_destroy_list, .print = ast_print_list, .init = ast_init_list },
        { .type = AST_IF,  .destroy = ast_destroy_if, .print = ast_print_if, .init = ast_init_if },
        { .type = AST_SIMPLE_CMD, .destroy = ast_destroy_simple_cmd, .print = ast_print_simple_cmd, .init = ast_init_simple_cmd }
};

#endif /* !AST_H */
