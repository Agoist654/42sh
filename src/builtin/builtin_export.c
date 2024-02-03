#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "exec/hash_map.h"
#include "exec/hash_map_fun.h"

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
    char *h_value = strdup(value);
    char *key = get_key(argv[1]);
    char *h_key = strdup(key);
    int ret = setenv(key, value, 1);
    ret = hash_map_insert(get_hm(), h_key, h_value, NULL);
    if (ret == 0)
        return 1;
    return 0;
}

static int unset_ret(char **argv, size_t name, char option)
{
    char *key = get_key(argv[name]);
    int ret = unsetenv(key);
    if (option == 'f')
        ret = hash_map_fun_remove(get_fun_hm(), key);
    else
        ret = hash_map_remove(get_hm(), key);
    if (ret == 0)
        return 1;
    return 0;
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
            return unset_ret(argv, 2, 'v');
        if (option_f)
            return unset_ret(argv, 2, 'f');
    }
    return unset_ret(argv, 1, 'n');
}
