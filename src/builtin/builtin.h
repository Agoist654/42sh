#ifndef BUILTIN_H
#define BUILTIN_H

typedef int (*builtin_f)(char **argv);

struct builtin
{
    char *command_name;
    builtin_f builtin;
};

struct error
{
    int e;
    int c;
    int b;
    int depth;
};

void init_error(void);
struct error *get_err(void);
void close_error(void);
int true_f(char **argv);
int false_f(char **argv);
int echo(char **argv);
int export_f(char **argv);
int unset_f(char **argv);
int exit_f(char **argv);
int continue_f(char **argv);
int break_f(char **argv);

#endif /* !BUILTIN_H */
