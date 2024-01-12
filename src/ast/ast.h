#ifndef AST_H
#define AST_H

#include "../lexer/lexer.h"

#define NB_AST 9

enum ast_type
{
    AST_LIST,
    AST_RULE_IF,
    AST_SIMPLE_COMMAND,
    AST_ELSE_CLAUSE,
    AST_SHELL_COMMAND,
    AST_AND_OR,
    AST_PIPELINE,
    AST_COMMAND,
    AST_COMPOUND_LIST
};

struct ast_list
{
    struct ast *current;
    struct ast *next;
};

struct ast_rule_if
{
    struct ast *cond;
    struct ast *then;
    struct ast *else_clause;
};

struct ast_else_clause
{
    struct ast *cond;
    struct ast *then;
    struct ast *else_clause;
};

struct ast_shell_command
{
    struct ast *rule_if;
};

struct ast_and_or
{
    struct ast *pipeline;
};

struct ast_pipeline
{
    struct ast *command;
};

struct ast_command
{
    struct ast *first;
};

struct ast_compound_list
{
    struct ast *and_or;
    struct ast *next;
};

struct ast_simple_command
{
    char **argv;
};

union ast_union
{
    struct ast_list ast_list;
    struct ast_rule_if ast_rule_if;
    struct ast_simple_command ast_simple_command;
    struct ast_else_clause ast_else_clause;
    struct ast_shell_command ast_shell_command;
    struct ast_and_or ast_and_or;
    struct ast_pipeline ast_pipeline;
    struct ast_command ast_command;
    struct ast_compound_list ast_compound_list;
};

struct ast;
typedef void (*destroy_f) (struct ast *ast);
typedef void (*print_f) (struct ast *ast);
typedef int (*exec_f) (struct ast *ast);

struct ftable
{
    destroy_f destroy;
    print_f print;
    exec_f exec;
};

struct ast
{
    enum ast_type type;
    union ast_union ast_union;
    struct ftable *ftable;
};

#endif /* !AST_H */
