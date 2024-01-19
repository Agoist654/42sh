#ifndef EXEC_H
#define EXEC_H

#include "ast/ast.h"

int ast_list_exec(struct ast *ast);
int ast_and_or_exec(struct ast *ast);
int ast_pipeline_exec(struct ast *ast);
int ast_command_exec(struct ast *ast);
int ast_simple_command_exec(struct ast *ast);
int ast_shell_command_exec(struct ast *ast);
int ast_rule_if_exec(struct ast *ast);
int ast_rule_while_exec(struct ast *ast);
int ast_rule_until_exec(struct ast *ast);
int ast_else_clause_exec(struct ast *ast);
int ast_compound_list_exec(struct ast *ast);

#endif /* !EXEC_H */
