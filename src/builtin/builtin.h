#ifndef BUILTIN_H
#define BUILTIN_H



typedef int (*int_void_f) (void);
typedef void (*void_ppchar_f) (char **argv);


struct builtin
{
    char *command_name;
    void_ppchar_f void_ppchar;
    int_void_f int_void;
};

int true_f(void);
int false_f(void);
void echo(char **argv);

#endif
