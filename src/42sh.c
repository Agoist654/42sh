#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "builtin/builtin.h"
#include "exec/exec.h"
#include "exec/hash_map.h"
#include "io_backend/io_backend.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

#include <stdlib.h>

int main(int argc, char *argv[])
{
    io_backend(argc, argv);
    struct lexer *lexer = lexer_init();
    int res = 0;
    while (lexer_peek(lexer).type != TOKEN_EOF)
    {
        struct ast *ast = parse_input(lexer);
        res = ast_list_exec(ast);
        ast_list_destroy(ast);
    }
    free(lexer_peek(lexer).buffer);
    free(lexer);
    io_close();
    hash_map_free(get_hm());
    return res;
}
