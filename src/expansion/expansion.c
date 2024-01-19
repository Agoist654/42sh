#include "expansion.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            //handle_double_quote
            continue;
        }
        else if (str[i] == 39) //39 is the ascii code for '
            simple_quote = !simple_quote;
        else if (str[i] == 34) //34 is the ascii code for "
            double_quote = !double_quote;
        else
            new_str[write++] = str[i];
    }
    free(str);
    new_str[write] = '\0';
    return new_str;
}
