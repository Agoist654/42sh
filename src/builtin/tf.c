#include "builtin.h"

int true_f(char **argv)
{
    (void)argv;
    return 0;
}

int false_f(char **argv)
{
    (void)argv;
    return 1;
}
