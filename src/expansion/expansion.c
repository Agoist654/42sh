#include "expansion.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *expansion(char *str)
{
    if (str == NULL || strlen(str) == 0)
    {
        return str;
    }
    if (str[0] != 39 /* && str[0] != '"' */)
    {
        return str;
    }
    else if (str[0] == 39)
    {
        // simple-quote
        int current = 0;
        char *ret_str = malloc(strlen(str) * sizeof(char));
        for (int i = 0; str[i] != '\0'; i++)
        {
            if (str[i] == 39)
            {
                continue;
            }
            ret_str[current] = str[i];
            current += 1;
        }
        ret_str[current] = '\0';
        free(str);
        return ret_str;
    }
    else
    {
        // double-quote
        return str;
    }
}
