#include <stdio.h>
#include <string.h>

int true_f(void)
{
    return 0;
}

int false_f(void)
{
    return 1;
}

static int len(char **argv)
{
    int len = 1;
    while (argv[len] != NULL)
        len++;
    return len;
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

void argvho(char **argv)
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
}
