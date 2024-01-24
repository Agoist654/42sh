//#define _POSIX_C_SOURCE 200112L
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include "pipeline.h"

 enum pipe_side
{
    LEFT,
    RIGHT
};

 pid_t exec_fork(struct ast *ast, int fds[2], enum pipe_side side)
{
    pid_t cpid = fork();
    if (cpid != 0)
        return cpid;

    int fd_to_replace = side == LEFT ? STDOUT_FILENO : STDIN_FILENO;
    int fd_pipe_end = side == LEFT ? fds[1] : fds[0];

    if (dup2(fd_pipe_end, fd_to_replace) == -1)
        errx(1, "dup2 failed\n");

    close(fds[0]);
    close(fds[1]);

    exit(ast->ftable->exec(ast));
    errx(1, "execvp failed\n");
}

 int exec_pipe(struct ast *ast)
{
    // create pipe
    int fds[2];
    if (pipe(fds) == -1)
        errx(1, "pipe failed");

    // exec each part with fork
    int pid_left = exec_fork(ast->ast_union.ast_pipeline.command, fds, LEFT);
    int pid_right = exec_fork(ast->ast_union.ast_pipeline.next, fds, RIGHT);

    // handle fork errors

    // close pipe
    close(fds[0]);
    close(fds[1]);

    // wait process
    int wstatus;
    waitpid(pid_left, &wstatus, 0);
    waitpid(pid_right, &wstatus, 0);

    return WEXITSTATUS(wstatus);
}
