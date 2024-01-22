#include "expansion.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void backslash(char *str, int *pos, char *new_str, int *write)
{
    if (str[*pos] == '\\' || str[*pos] == '$' || str[*pos] == '`' || str[*pos] == '"')
        new_str[*write += 1] = str[*pos];
    else if (str[*pos] != '\n')
    {
        new_str[*write += 1] = '\\';
        new_str[*write += 1] = str[*pos];
    }
    *pos += 1;
}

char *expansion(char *str)
{
    if (str == NULL)
        return str;
    int simple_quote = 0;
    int double_quote = 0;
    char *new_str = calloc(512, 1);
    if (new_str == NULL)
        return str;
    int write = 0;
    int current_size = 512;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (write == current_size - 1)
        {
            new_str = realloc(new_str, 2 * current_size);
            current_size *= 2;
        }
        if (simple_quote && str[i] != 39)
            new_str[write++] = str[i];
        else if (double_quote && str[i] != 39)
        {
            if (str[i] == '\\')
            {
                i++;
                backslash(str, &i, new_str, &write);
            }
            // handle_double_quote
            continue;
        }
        else if (str[i] == 39) // 39 is the ascii code for '
            simple_quote = !simple_quote;
        else if (str[i] == 34) // 34 is the ascii code for "
            double_quote = !double_quote;
        else
            new_str[write++] = str[i];
    }
    free(str);
    new_str[write] = '\0';
    return new_str;
}
