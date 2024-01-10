
#ifndef AST_H
#define AST_H


#define NB_AST 3

enum ast_type
{
    AST_LIST,
    AST_IF,
    AST_SIMPLE_CMD
}

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
    enum ast_type;
    int nb_cmd;
    void **cmd_list;
};

union ast_union
{
    struct ast_list;
    struct ast_if;
    struct ast_simple_cmd;
};


typedef void (*destroyer) (struct ast *ast);
typedef void (*printer) (struct ast *ast);
typedef (struct ast *)(*initer) (void);
struct ast
{
    enum ast_type type;
    union ast_union ast_union;
    destroyer destroy;
    printer print;
    initer init;
};


static struct ast asts[NB_AST]
{
    {
        { .type = AST_LIST, .ast_union = ast_list, .destroyer = ast_destroy_list, .printer = ast_print_list, .initer = ast_init_list },
        { .type = AST_IF, .ast_union = ast_if, .destroyer = ast_destroy_if, .printer = ast_print_if, .initer = ast_init_if },
        { .type = AST_SIMPLE_CMD, .ast_union = ast_simple_cmd, .destroyer = ast_destroy_simple_command, .printer = ast_print_simple_command, .initer = ast_init_simple_command }
    };
}

#endif /* !AST_H */
