#define _POSIX_C_SOURCE 200809L
#include "redirection.h"

#include <ctype.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast.h"
#include "dlist.h"
#define NB_REDIRECTION 4

static void my_close(int fd1, int fd2, int fd3)
{
    if (fd1 != -1)
        close(fd1);
    if (fd2 != -1)
        close(fd2);
    if (fd3 != -1)
        close(fd3);
}

typedef int (*redirection_f)(int io_number, char *word);

static int redirection_right(int io_number, char *word);
static int redirection_left(int io_number, char *word);
static int redirection_right_right(int io_number, char *word);
static int redirection_right_pipe(int io_number, char *word);

redirection_f redirections[/*NB_REDIRECTION*/] = {
    [TOKEN_REDIRECTION_RIGHT - TOKEN_REDIRECTION_RIGHT] = redirection_right,
    [TOKEN_REDIRECTION_LEFT - TOKEN_REDIRECTION_RIGHT] = redirection_left,
    [TOKEN_REDIRECTION_RIGHT_RIGHT - TOKEN_REDIRECTION_RIGHT] =
        redirection_right_right,
    [TOKEN_REDIRECTION_RIGHT_PIPE - TOKEN_REDIRECTION_RIGHT] = redirection_right_pipe
    //[TOKEN_REDIRECTION_] =  ,
    //[TOKEN_REDIRECTION_] =  ,
    //[TOKEN_REDIRECTION_] =  ,
    //[TOKEN_REDIRECTION_] =
};

//static
int isnumber(char *word)
{
    for (size_t k = 0; k < strlen(word); k++)
    {
        if (!isdigit(word[k]))
            return 0;
    }
    return 1;
}

int exec_redirection(struct dlist *dlist, struct redirection *redir)
{
    int io_number_int = 0;
    if (redir->io_number == NULL)
        io_number_int = -1;
    else
        io_number_int = atoi(redir->io_number);

    int save_fd = 0;
    for (enum token_type op = 0; op < NB_REDIRECTION; op++)
    {
        if (op == redir->op)
        {
            save_fd = redirections[op](io_number_int, redir->word);
            if (save_fd == -1)
                return -1;
            dlist_push_back(dlist, save_fd, io_number_int);
        }
    }
    return 1;
}

int restore_redirection(struct dlist *dlist)
{
    if (!dlist->head)
        return 1;
    struct dlist_item *tmp = dlist->head;
    while (tmp->prev != NULL)
    {
        if (dup2(tmp->save_fd, tmp->io_number) == -1)
        {
            dlist_close_fd(dlist);
            return -1;
        }
        tmp = tmp->prev;
    }
    dlist_close_fd(dlist);
    dlist_destroy(dlist);
    return 1;
}

// for save -> dup2(save_fd, io_number)
static int redirection_right(int io_number, char *word)
{
    int fd = open(word, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd == -1)
        return -1;
    if (io_number == -1)
        io_number = STDOUT_FILENO;

    int save_fd = dup(io_number);
    if (save_fd == -1)
    {
        close(fd);
        return -1;
    }

    int new_io_number = dup2(fd, io_number);
    if (new_io_number == -1)
    {
        my_close(-1, save_fd, fd);
        return -1;
    }
    return save_fd;
}

static int redirection_right_right(int io_number, char *word)
{
    if (io_number == -1)
        io_number = STDOUT_FILENO;
    int save_fd = dup(io_number);
    if (save_fd == -1)
        return -1;
    int fd = open(word, O_CREAT | O_TRUNC | O_APPEND, 0644);
    if (fd == -1)
    {
        close(save_fd);
        return -1;
    }

    int new_io_number = dup2(fd, io_number);
    if (new_io_number == -1)
    {
        my_close(-1, save_fd, fd);
        return -1;
    }
    return save_fd;
}

static int redirection_left(int io_number, char *word)
{
    if (io_number == -1)
        io_number = STDOUT_FILENO;
    int save_fd = dup(io_number);
    if (save_fd == -1)
        return -1;
    int fd = open(word, O_RDONLY, 0644);
    if (fd == -1)
    {
        close(save_fd);
        return -1;
    }

    int new_io_number = dup2(fd, io_number);
    if (new_io_number == -1)
    {
        my_close(-1, save_fd, fd);
        return -1;
    }
    return save_fd;
}

//static
int redirection_right_pipe(int io_number, char *word)
{
    int fd = open(word, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd == -1)
        return -1;
    if (io_number == -1)
        io_number = STDOUT_FILENO;

    int save_fd = dup(io_number);
    if (save_fd == -1)
    {
        close(fd);
        return -1;
    }

    int new_io_number = dup2(fd, io_number);
    if (new_io_number == -1)
    {
        my_close(-1, save_fd, fd);
        return -1;
    }
    return save_fd;
}

//static
int redirection_left_and(int io_number, char *word)
{
    if (strcmp(word, "-") == 0)
    {
        close(io_number);
        return io_number;
    }
    int fd = atoi(word);
    FILE *test = fdopen(fd, "r");
    if (test == NULL)
        return -1;
    if (io_number == -1)
        io_number = STDIN_FILENO;

    int save_fd = dup(io_number);
    if (save_fd == -1)
    {
        close(fd);
        return -1;
    }

    int new_io_number = dup2(fd, io_number);
    if (new_io_number == -1)
    {
        my_close(-1, save_fd, fd);
        return -1;
    }
    return save_fd;
}

//static
int redirection_right_and(int io_number, char *word)
{
    // if (isnumber(word))
    //     return -1;

    if (strcmp(word, "-") == 0)
    {
        close(io_number);
        return io_number;
    }
    int fd = atoi(word);
    FILE *test = fdopen(fd, "w");
    if (test == NULL)
        return -1;
    if (io_number == -1)
        io_number = STDOUT_FILENO;

    int save_fd = dup(io_number);
    if (save_fd == -1)
    {
        close(fd);
        return -1;
    }

    int new_io_number = dup2(fd, io_number);
    if (new_io_number == -1)
    {
        my_close(-1, save_fd, fd);
        return -1;
    }
    return save_fd;
}
