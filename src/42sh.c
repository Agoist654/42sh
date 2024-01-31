#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "builtin/builtin.h"
#include "exec/exec.h"
#include "exec/hash_map.h"
#include "exec/hash_map_fun.h"
#include "io_backend/io_backend.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char *argv[])
{
    io_backend(argc, argv);
    struct lexer *lexer = lexer_init();
    int res = 0;
    while (lexer_peek(lexer).type != TOKEN_EOF)
    {
        struct ast *ast = parse_input(lexer);
        if (ast == NULL)
            continue;
        res = ast_list_exec(ast, argv);
        ast_list_destroy(ast);
    }
    free(lexer_peek(lexer).buffer);
    free(lexer_look_ahead(lexer).buffer);
    free(lexer);
    io_close();
    hash_map_free(get_hm());
    hash_map_fun_free(get_fun_hm());
    return res;
}
