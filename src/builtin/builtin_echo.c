#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"

static int len(char **argv)
{
    int len = 1;
    while (argv[len] != NULL)
        len++;
    return len;
}

static void print(char *argv, size_t len)
{
    for (size_t dep = 0; dep < len; dep++)
    {
        if ((dep + 1) < len && argv[dep] == '\\' && argv[dep + 1] == 'n')
        {
            dep++;
            printf("\\n");
        }
        else if ((dep + 1) < len && argv[dep] == '\\' && argv[dep + 1] == 't')
        {
            dep++;
            printf("\\t");
        }
        else if (argv[dep] == '\\')
            printf("\\");
        else
            printf("%c", argv[dep]);
    }
}

static void e_print(char *argv, size_t len)
{
    for (size_t dep = 0; dep < len; dep++)
    {
        if ((dep + 1) < len && argv[dep] == '\\' && argv[dep + 1] == 'n')
        {
            dep++;
            printf("\n");
        }
        else if ((dep + 1) < len && argv[dep] == '\\' && argv[dep + 1] == 't')
        {
            dep++;
            printf("\t");
        }
        else if (argv[dep] == '\\')
        {
            dep++;
            printf("%c", argv[dep]);
        }
        else
            printf("%c", argv[dep]);
    }
}

static void prt(char **argv, int pl, int eE)
{
    if (eE)
        e_print(argv[pl], strlen(argv[pl]));
    else
        print(argv[pl], strlen(argv[pl]));
    pl++;
    while (pl < len(argv))
    {
        printf(" ");
        if (eE)
            e_print(argv[pl], strlen(argv[pl]));
        else
            print(argv[pl], strlen(argv[pl]));
        pl++;
    }
}

int echo(char **argv)
{
    int pl = 1;
    size_t pos = 0;
    int n = 1;
    int tmpn = 1;
    int eE = 0;
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
