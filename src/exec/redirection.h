#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "ast/ast.h"
#include "dlist.h"

int exec_redirection(struct dlist *list, struct redirection *redir);
int restore_redirection(struct dlist *dlist);

#endif // REDIRECTION_H
