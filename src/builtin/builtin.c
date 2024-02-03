#include "builtin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct error *error = NULL;

void init_error(void)
{
    if (error == NULL)
    {
        error = malloc(sizeof(struct error));
        if (error == NULL)
            return;
        error->e = 0;
        error->c = 0;
        error->b = 0;
        error->depth = 0;
    }
    return;
}

void close_error(void)
{
    if (error != NULL)
    {
        free(error);
        error = NULL;
    }
    return;
}

struct error *get_err(void)
{
    return error;
}

int is_number(char *str)
{
    if (str == NULL)
        return 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] < '0' || str[i] > '9')
            return 0;
    }
    return 1;
}

int exit_f(char **argv)
{
    error->e = 1;
    if (argv[1] != NULL && is_number(argv[1]))
        return atoi(argv[1]);
    return -1;
}

int continue_f(char **argv)
{
    if (argv[1] != NULL)
    {
        if (is_number(argv[1]))
        {
            error->c = atoi(argv[1]);
            if (error->c == 0)
                return 1;
            return 0;
        }
        return 1;
    }
    error->c = 1;
    return 0;
}

int break_f(char **argv)
{
    if (argv[1] != NULL)
    {
        if (is_number(argv[1]))
        {
            error->b = atoi(argv[1]);
            if (error->b == 0)
                return 1;
            return 0;
        }
        return 1;
    }
    error->b = 1;
    return 0;
}
