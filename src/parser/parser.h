#ifndef PARSER_H
#define PARSER_H

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

#endif /* !PARSER_H */
