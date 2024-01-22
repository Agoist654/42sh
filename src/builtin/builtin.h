#ifndef BUILTIN_H
#define BUILTIN_H

typedef int (*builtin_f)(char **argv);


struct builtin
{
    char *command_name;
    builtin_f builtin;
};

int true_f(char **argv);
int false_f(char **argv);
int echo(char **argv);

#endif /* !BUILTIN_H */
