#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static int len(char **argv)
{
    int len = 1;
    while (argv[len] != NULL)
        len++;
    return len;
}

static char *get_value(char *buffer)
{
    return strchr(buffer, '=') + 1;
}

static size_t get_equal_index(char *buffer)
{
    size_t index = 0;
    for (; index < strlen(buffer); index++)
    {
        if (buffer[index] == '=')
            break;
    }
    return index;
}

static char *get_key(char *buffer)
{
    size_t equal_index = get_equal_index(buffer);
    buffer[equal_index] = '\0';
    return buffer;
}

int export_f(char **argv)
{
    char *value = get_value(argv[1]);
    char *key = get_key(argv[1]);
    int ret = setenv(key, value, 1);
    if (ret != 0)
        return 1;
    return ret;
}

static int unset_ret(char **argv, size_t name)
{
    char *key = get_key(argv[name]);
    int ret = unsetenv(key);
    if (ret != 0)
        return 1;
    return ret;
}

int unset_f(char **argv)
{
    int option_v = 0;
    int option_f = 0;

    if (argv[1][0] == '-')
    {
        for (size_t tmp = 1; tmp < strlen(argv[1]); tmp++)
        {
            if (option_v && option_f)
                return 1;
            if (argv[1][tmp] == 'v')
                option_v = 1;
            else if (argv[1][tmp] == 'f')
                option_f = 1;
            else
                return 1;
        }
        if (option_v)
            return unset_ret(argv, 2);
    }
    return unset_ret(argv, 1);
}

static void prt(char **argv, int pl, int eE)
{
    if (eE)
        printf("%s", argv[pl]);
    else
    {
        for (size_t dep = 0; dep < strlen(argv[pl]); dep++)
        {
            if (argv[pl][dep] == '\n')
                printf("\\n");
            else if (argv[pl][dep] == '\t')
                printf("\\t");
            else if (argv[pl][dep] == '\\')
                printf("\\");
            else
                printf("%c", argv[pl][dep]);
        }
    }
    pl++;
    while (pl < len(argv))
    {
        if (argv[pl][0] == '\0')
            continue;
        printf(" ");
        if (eE)
            printf("%s", argv[pl]);
        else
        {
            for (size_t dep = 0; dep < strlen(argv[pl]); dep++)
            {
                if (argv[pl][dep] == '\n')
                    printf("\\n");
                else if (argv[pl][dep] == '\t')
                    printf("\\t");
                else if (argv[pl][dep] == '\\')
                    printf("\\");
                else
                    printf("%c", argv[pl][dep]);
            }
        }
        pl++;
    }
}

int echo(char **argv)
{
    int pl = 1;
    size_t pos = 0;
    int n = 1;
    int tmpn = 1;
    int eE = 1;
    int valid = 0;
    int same_n = 0;

    while (pl < len(argv))
    {
        if (argv[pl][pos] == '-')
        {
            pos++;
            while (pos != strlen(argv[pl]))
            {
                char tmp = argv[pl][pos++];
                if (tmp == 'n')
                {
                    n = 0;
                    tmpn = 0;
                }
                else if (tmp == 'e')
                    eE = 1;
                else if (tmp == 'E')
                    eE = 0;
                else
                {
                    valid = 1;
                    if (!tmpn && !same_n)
                    {
                        tmpn = 1;
                        n = 1;
                    }
                    break;
                }
            }
        }

        pos = 0;

        if (!n)
            same_n = 1;
        if (valid || argv[pl][pos] != '-')
        {
            prt(argv, pl, eE);
            break;
        }
        pl++;
    }
    if (n)
        printf("\n");
    return 0;
}
