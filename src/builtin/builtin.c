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

//static 
int space(char *ec, int pos, int len)
{
    while (ec[pos] == ' ' && pos != len)
        pos++;
    return pos;
}

static int len(char **ec)
{
    int len = 1;
    while (ec[len] != NULL)
        len++;
    return len;
}

static void prt(char **ec, int pl, int eE)
{
    if (eE)
        printf("%s", ec[pl]);
    else
    {
        for (size_t dep = 0; dep < strlen(ec[pl]); dep++)
        {
            if (ec[pl][dep] == '\n')
                printf("\\n");
            else if (ec[pl][dep] == '\t')
                printf("\\t");
            else if (ec[pl][dep] == '\\')
                printf("\\");
            else
                printf("%c", ec[pl][dep]);
        }
    }
    pl++;
    while (pl < len(ec))
    {
        printf(" ");
        if (eE)
            printf("%s", ec[pl]);
        else
        {
            for (size_t dep = 0; dep < strlen(ec[pl]); dep++)
            {
                if (ec[pl][dep] == '\n')
                    printf("\\n");
                else if (ec[pl][dep] == '\t')
                    printf("\\t");
                else if (ec[pl][dep] == '\\')
                    printf("\\");
                else
                    printf("%c", ec[pl][dep]);
            }
        }
        pl++;
    }
}

void echo(char **ec)
{
    int pl = 1;
    size_t pos = 0;
    int n = 1;
    int tmpn = 0;
    int eE = 1;
    int valid = 0;

    while (pl < len(ec))
    {
        if (ec[pl][pos] == '-')
        {
            pos++;
            while (pos != strlen(ec[pl]))
            {
                char tmp = ec[pl][pos++];
                if (tmp == 'n')
                {
                    n = 0;
                    tmpn = 1;
                }
                else if (tmp == 'e')
                    eE = 1;
                else if (tmp == 'E')
                    eE = 0;
                else
                {
                    valid = 1;
                    if (tmpn == 1)
                        n = 1;
                    break;
                }
            }
        }
        pos = 0;
        tmpn = 0;
        if (valid || ec[pl][pos] != '-')
        {
            prt(ec, pl, eE);
            break;
        }
        pl++;
    }
    if (n)
        printf("\n");
}
