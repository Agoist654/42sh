#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

struct error
{
    int res;
    char *msg;
};

struct ast *parse_input(struct lexer *lexer);

#endif /* !PARSER_H */
