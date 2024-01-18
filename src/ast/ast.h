#ifndef AST_H
#define AST_H

#include "lexer/lexer.h"

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
    AST_COMPOUND_LIST,
    AST_RULE_WHILE,
    AST_RULE_UNTIL,
    AST_RULE_FOR
};

enum element_type
{
    WORD,
    REDIR
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
    struct ast *next;
};

struct ast_pipeline
{
    struct ast *command;
    struct ast *next;
    int neg;
};

struct ast_command
{
    struct ast *first;
    struct redirection **redirection;
};

struct ast_compound_list
{
    struct ast *and_or;
    struct ast *next;
};

struct ast_simple_command
{
    char **argv;
    size_t len_argv;
    struct redirection **redirection;
    size_t len_redir;
};

struct ast_rule_while
{
    struct ast *cond;
    struct ast *then;
};

struct ast_rule_until
{
    struct ast *cond;
    struct ast *then;
};

struct ast_rule_for
{
    struct ast *count;
    struct ast *then;
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
    struct ast_rule_while ast_rule_while;
    struct ast_rule_until ast_rule_until;
    struct ast_rule_for ast_rule_for;
};

union element_union
{
    char *word;
    struct redirection *redir;
};

struct ast;
typedef void (*destroy_f)(struct ast *ast);
typedef void (*print_f)(struct ast *ast);
typedef int (*exec_f)(struct ast *ast);

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

struct element
{
    enum element_type type;
    union element_union;
};

struct redirection
{
    char *io_number;
    enum token_type op;
    char *word;
};

void ast_list_destroy(struct ast *ast);
void ast_list_print(struct ast *ast);
struct ast *ast_init(enum ast_type type);

#endif /* !AST_H */
