#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "io_backend/io_backend.h"
#include "../io_backend/io_backend.h"

#define BUFFER_SIZE 512

// static struct token current_token = {
//     .type = TOKEN_NULL,
//     .buffer = NULL,
//     .len = BUFFER_SIZE
// };

static struct token realloc_buffer(struct token token)
{
    if (strlen(token.buffer) == token.len - 1)
    {
        token.buffer = realloc(token.buffer, 2 * token.len * sizeof(char));
        for (size_t k = token.len; k < token.len * 2; k++)
        {
            token.buffer[k] = '\0';
        }
        token.len = token.len * 2;
    }
    return token;
}

static struct token io_eat(struct token token)
{
    if (strlen(token.buffer) == token.len - 1)
    {
        token.buffer = realloc(token.buffer, 2 * token.len * sizeof(char));
        for (size_t k = token.len; k < token.len * 2; k++)
        {
            token.buffer[k] = '\0';
        }
        token.len = token.len * 2;
    }
    token.buffer[strlen(token.buffer)] = io_pop();
    return token;
}

static char *tokens[NB_TOKENS] = {
    // step 1
    [TOKEN_IF] = "if",
    [TOKEN_THEN] = "then",
    [TOKEN_ELIF] = "elif",
    [TOKEN_ELSE] = "else",
    [TOKEN_FI] = "fi",
    [TOKEN_SEMICOLON] = ";",
    [TOKEN_NEWLINE] = "\n",

    // step 3
    [TOKEN_BRACKET_LEFT] = "{",
    [TOKEN_BRACKET_RIGHT] = "}",
    [TOKEN_PARENTHESIS_LEFT] = "(",
    [TOKEN_PARENTHESIS_RIGHT] = ")",

    // step2
    [TOKEN_WHILE] = "while",
    [TOKEN_FOR] = "for",
    [TOKEN_UNTIL] = "until",
    [TOKEN_DO] = "do",
    [TOKEN_DONE] = "done",
    [TOKEN_IN] = "in",
    [TOKEN_PIPE] = "|",
    [TOKEN_NEGATION] = "!",
    [TOKEN_AND] = "&&",
    [TOKEN_OR] = "||",
    [TOKEN_REDIRECTION_RIGHT] = ">",
    [TOKEN_REDIRECTION_LEFT] = "<",
    [TOKEN_REDIRECTION_RIGHT_RIGHT] = ">>",
    [TOKEN_REDIRECTION_RIGHT_AND] = ">&",
    [TOKEN_REDIRECTION_LEFT_AND] = "<&",
    [TOKEN_REDIRECTION_RIGHT_PIPE] = ">|",
    [TOKEN_REDIRECTION_LEFT_RIGHT] = "<>"

};

static struct token token_error = { .type = TOKEN_ERROR,
                                    .buffer = NULL,
                                    .len = BUFFER_SIZE };

static struct token token_null = { .type = TOKEN_NULL,
                                   .buffer = NULL,
                                   .len = BUFFER_SIZE };

static int isassignment_word(char *buffer)
{
    if (buffer && isdigit(buffer[0]))
        return 0;
    size_t index = 0;
    while (index < strlen(buffer))
    {
        if (buffer[index] == '=')
            break;
        index++;
    }

    if (index == strlen(buffer))
        return 0;

    for (size_t k = 0; k < index; k++)
    {
        if (buffer[k] != '_' && !isalnum(buffer[k]))
            return 0;
    }
    return 1;
}

static int isnumber(char *buffer)
{
    for (size_t k = 0; k < strlen(buffer); k++)
    {
        if (!isdigit(buffer[k]))
            return 0;
    }
    return 1;
}

static struct token token_init(void)
{
    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    if (!buffer)
        return token_error;
    struct token res = { .type = TOKEN_NULL,
                         .buffer = buffer,
                         .len = BUFFER_SIZE };
    return res;
}

struct lexer *lexer_init(void)
{
    struct lexer *res = malloc(sizeof(struct lexer));
    if (!res)
        return NULL;
    res->current_token = token_null;
    res->look_ahead = token_null;
    return res;
}

static void handle_comment(void)
{
    while (io_peek() != EOF && io_peek() != '\n')
        io_pop();
    return;
}

static int isquote(char io_peek)
{
    if (io_peek == '\'' || io_peek == '\"')
        return 1;
    return 0;
}

static int quote_is_escaped(char *buffer)
{
    size_t size = strlen(buffer);
    int res = 0;
    for (size_t k = size - 1; size - k != size + 1; k--)
    {
        if (buffer[k] != '\\')
            return res;
        res = !res;
    }
    return res;
}

static int handle_single_quote(struct token token)
{
    // append the quote in the buffer
    token = io_eat(token);
    token.type = TOKEN_SINGLE_QUOTE;
    while (io_peek() != EOF)
    {
        // token.buffer[strlen(token.buffer) - 1] != '\\')
        if (io_peek() == '\'' && !quote_is_escaped(token.buffer))
        {
            token = io_eat(token);
            return TOKEN_SINGLE_QUOTE;
        }
        token = io_eat(token);
    }
    if (token.type == TOKEN_SINGLE_QUOTE)
        return TOKEN_SINGLE_QUOTE;
    return TOKEN_ERROR;
}

static int handle_double_quote(struct token token)
{
    // append the quote in the buffer
    token = io_eat(token);
    token.type = TOKEN_DOUBLE_QUOTE;
    while (io_peek() != EOF)
    {
        // token.buffer[strlen(token.buffer) - 1] != '\\')
        if (io_peek() == '\"' && !quote_is_escaped(token.buffer))
        {
            token = io_eat(token);
            return TOKEN_DOUBLE_QUOTE;
        }
        token = io_eat(token);
    }
    if (token.type == TOKEN_DOUBLE_QUOTE)
        return TOKEN_DOUBLE_QUOTE;
    return TOKEN_ERROR;
}

static int handle_quote(struct token token)
{
    if (io_peek() == '\'')
        return handle_single_quote(token);
    if (io_peek() == '\"')
        return handle_double_quote(token);
    return TOKEN_ERROR;
}

static int ispart_prev_op(char io_peek, char *buffer)
{
    if (buffer[strlen(buffer) - 1] == '<' && (io_peek == '&' || io_peek == '>'))
        return 1;
    if (buffer[strlen(buffer) - 1] == '>'
        && (io_peek == '>' || io_peek == '&' || io_peek == '|'))
        return 1;
    if (buffer[strlen(buffer) - 1] == '&' && io_peek == '&')
        return 1;
    if (buffer[strlen(buffer) - 1] == '|' && io_peek == '|')
        return 1;
    return 0;
}

static int isoperator(char io_peek)
{
    if (io_peek == ';' || io_peek == '\n' || io_peek == '<' || io_peek == '>'
        || io_peek == '&' || io_peek == '|' || io_peek == '(' || io_peek == ')')
        return 1;
    return 0;
}

static int isredir(char io_peek)
{
    if (io_peek == '>' || io_peek == '<')
        return 1;
    return 0;
}

static int isdelimiter(char io_peek)
{
    if (io_peek == ' ' || io_peek == EOF || io_peek == '\n' || io_peek == ';')
        return 1;
    return 0;
}

static int isnotquoted(struct token token)
{
    if (token.type != TOKEN_SINGLE_QUOTE && token.type != TOKEN_DOUBLE_QUOTE)
        return 1;
    return 0;
}

static struct token handle_backslash(struct token token)
{
    io_pop();
    if (io_peek() != '\n')
    {
        token = realloc_buffer(token);
        token.buffer[strlen(token.buffer)] = '\\';
        token = realloc_buffer(token);
        io_eat(token);
    }
    else
        io_pop();
    return token;
}

static struct token token_reg(struct token res)
{
    // rule 1
    while (io_peek() != EOF)
    {
        // rule 2: if peek is part of the current operator and not quoted
        if (res.type != TOKEN_SINGLE_QUOTE && res.type == TOKEN_OPERATOR
            && ispart_prev_op(io_peek(), res.buffer))
        {
            res = io_eat(res);
            continue;
        }

        //  //rule 3: if peek is not part of current operator return the token
        //  (the operator)
        else if (res.type == TOKEN_OPERATOR
                 && !ispart_prev_op(io_peek(), res.buffer))
            return res;

        // step1  // rule 4: backslash/quote/double-quote and not quoted
        if (isnotquoted(res) && (isquote(io_peek()) || io_peek() == '\\'))
        {
            if (isquote(io_peek()))
            {
                res.type = handle_quote(res);
                if (isdelimiter(io_peek()))
                    return res;
            }
            else
                res = handle_backslash(res);
            continue;
        }

        // rule 5: sub shell
        // step1   //rule 6: if not quoted therfore: start of a new operator
        if (isoperator(io_peek()))
        {
            if (res.type == TOKEN_WORD)
            {
                if (isnumber(res.buffer) && isredir(io_peek()))
                    res.type = TOKEN_IONUMBER;
                return res;
            }
            res.type = TOKEN_OPERATOR;
            res = io_eat(res);
            continue;
            // return res;
        }

        // step1   //rule 7: if not quoted : is a >blank< return token
        if (isblank(io_peek()))
        {
            io_pop();
            if (strcmp(res.buffer, "") != 0 && strcmp(res.buffer, " ") != 0)
                return res;
            continue;
        }
        // step1   //rule 8: if TOKEN_WORD : res.buffer[strlen(res.buffer)] =
        //    io_pop();
        if (res.type == TOKEN_WORD)
            res = io_eat(res);
        // step1   //rule 9: comment until \n : TOKEN_COMMENT, the \n is part of
        // it
        else if (res.type != TOKEN_SINGLE_QUOTE && io_peek() == '#')
        {
            handle_comment();
            continue;
        }
        // step1   //rule 10: start of a TOKEN_WORD
        res.type = TOKEN_WORD;
    }
    if (res.type != TOKEN_WORD && res.type != TOKEN_OPERATOR)
        res.type = TOKEN_EOF;
    return res;
}

// static
struct token lex(void)
{
    struct token res = token_init();
    res = token_reg(res);
    if (res.type == TOKEN_WORD && isassignment_word(res.buffer))
        res.type = TOKEN_ASSIGNMENT_WORD;
    if (res.type != TOKEN_WORD && res.type != TOKEN_OPERATOR)
        return res;
    // for (enum token_type type = TOKEN_IF; type < NB_TOKENS; type++)
    for (int type = 0; type < NB_TOKENS; type++)
    {
        if (tokens[type] && strcmp(tokens[type], res.buffer) == 0)
        {
            res.type = type;
            break;
        }
    }
    if (res.type == TOKEN_OPERATOR)
        res.type = TOKEN_ERROR;
    return res;
}

struct token lexer_peek(struct lexer *lexer)
{
    if (lexer->current_token.type == TOKEN_NULL)
        lexer->current_token = lex();
    if (lexer->look_ahead.type == TOKEN_NULL)
        lexer->look_ahead = lex();
    return lexer->current_token;
}

/*
 * marche uniquement si on a fait un lexer_peek() avant
 * si non renvoie un TOKEN_NULL au premier appel
 */
struct token lexer_pop(struct lexer *lexer)
{
    struct token save_token = lexer->current_token;
    lexer->current_token = lexer->look_ahead;
    lexer->look_ahead = lex();
    return save_token;
}

struct token lexer_look_ahead(struct lexer *lexer)
{
    if (lexer->look_ahead.type == TOKEN_NULL)
    {
        lexer->look_ahead = lex();
    }
    return lexer->look_ahead;
}
