
#include "io_backend/io_backend.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "builtin/builtin.h"
#include "exec/exec1.h"
#include "ast/ast.h"


int main(int argc, char *argv[])
{
    io_backend(argc, argv);
    struct lexer *lexer = lexer_init();
    struct ast *ast = parse_input(lexer);
    int res = ast_list_exec(ast);
    //ast_list_destroy(ast);
    io_close();
    return res;
}
