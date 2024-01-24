#define STR_SIZE 512

#include "expansion.h"
#include "exec/hash_map.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char special[] = {
    '@',
    '*',
    '?',
    '$',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '#',
    -1
};

static void realloc_str(struct str *str)
{
    if (str->current_pos == str->size - 1)
    {
        str->str = realloc(str->str, str->size * 2 * sizeof(char));
        str->size *= 2;
    }
    return;
}

static void backslash(struct str *str,  struct str *new_str)
{
    if (str->str[str->current_pos] == '\\' || str->str[str->current_pos] == '$' || str->str[str->current_pos] == '`'
        || str->str[str->current_pos] == '"')
        new_str->str[new_str->current_pos++] = str->str[str->current_pos];
    else if (str->str[str->current_pos] != '\n')
    {
        new_str->str[new_str->current_pos++] = '\\';
        realloc_str(new_str);
        new_str->str[new_str->current_pos++] = str->str[str->current_pos];
    }
    str->current_pos++;
}

static int is_special(char c)
{
    for (int i = 0; special[i] != -1; i++)
    {
        if (special[i] == c)
            return 1;
    }
    return 0;
}

static struct str get_var_name(struct str *str)
{
    char *name = calloc(STR_SIZE, sizeof(char));
    struct str key = { .str = name, .current_pos = 0, .size = STR_SIZE };
    if (key.str == NULL)
        return key;
    if (is_special(str->str[str->current_pos]) && str->str[str->current_pos] != '{')
    {
        key.str[key.current_pos++] = str->str[str->current_pos++];
        return key;
    }
    if (str->str[str->current_pos] == '{')
    {
        str->current_pos++;
        while (str->str[str->current_pos] != '}' && str->current_pos != '\0')
        {
            realloc_str(&key);
            key.str[key.current_pos++] = str->str[str->current_pos++];
        }
        if (str->str[str->current_pos] != '\0')
            str->current_pos++;
        key.str[key.current_pos] = '\0';
        return key;
    }
    if (str->str[str->current_pos] == '_' || isalpha(str->str[str->current_pos]))
    {
        key.str[key.current_pos++] = str->str[str->current_pos++];
        while (isalnum(str->str[str->current_pos]) || str->str[str->current_pos] == '_')
        {
            realloc_str(&key);
            key.str[key.current_pos++] = str->str[str->current_pos++];
        }
        key.str[key.current_pos] = '\0';
        return key;
    }
    key.str[key.current_pos] = '\0';
    return key;
}

static void handle_var(struct str *str, struct str *new_str)
{
    str->current_pos++;
    struct str key = get_var_name(str);
    if (key.str == NULL || key.str[0] == '\0')
    {
        new_str->str[new_str->current_pos++] = '$';
        free(key.str);
        return;
    }
    else
    {
        // handle_special_var
        char *value = hash_map_get(get_hm(), key.str);
        if (value == NULL)
        {
            free(key.str);
            return;
        }
        else
        {
            for (int i = 0; value[i] != '\0'; i++)
            {
                realloc_str(new_str);
                new_str->str[new_str->current_pos++] = value[i];
            }
            free(key.str);
            return;
        }
    }
    return;
}

char *expansion(char *str_init)
{
    if (str_init == NULL)
        return str_init;
    int simple_quote = 0;
    int double_quote = 0;
    char *new_str_init = calloc(STR_SIZE, sizeof(char));
    if (new_str_init == NULL)
        return str_init;
    struct str str = { .str = str_init, .current_pos = 0, .size = STR_SIZE };
    struct str new_str = { .str = new_str_init, .current_pos = 0, .size = STR_SIZE };
    while (str.str[str.current_pos] != '\0')
    {
        realloc_str(&new_str);
        if (simple_quote && str.str[str.current_pos] != '\'')
        {
            new_str.str[new_str.current_pos++] = str.str[str.current_pos++];
            continue;
        }
        else if (str.str[str.current_pos] == '$')
            handle_var(&str, &new_str);
        else if (double_quote && str.str[str.current_pos] != '"')
        {
            if (str.str[str.current_pos] == '\\')
            {
                str.current_pos++;
                backslash(&str, &new_str);
            }
            str.current_pos++;
            continue;
        }
        else if (str.str[str.current_pos] == '\'')
        {
            simple_quote = !simple_quote;
            str.current_pos++;
            continue;
        }
        else if (str.str[str.current_pos] == '"')
        {
            double_quote = !double_quote;
            str.current_pos++;
            continue;
        }
        else
            new_str.str[new_str.current_pos++] = str.str[str.current_pos++];
    }
    free(str.str);
    new_str.str[new_str.current_pos] = '\0';
    return new_str.str;
}
