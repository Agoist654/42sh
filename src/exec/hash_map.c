#define _XOPEN_SOURCE 500
#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expansion/expansion.h"

#define HASHMAP_SIZE 8

static struct hash_map *hm = NULL;

struct hash_map *get_hm(void)
{
    if (hm == NULL)
        hm = hash_map_init(HASHMAP_SIZE);
    return hm;
}

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *map = malloc(sizeof(struct hash_map));
    if (map == NULL)
    {
        return NULL;
    }
    map->data = malloc(size * sizeof(struct pair_list *));
    if (map->data == NULL)
    {
        free(map);
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        map->data[i] = NULL;
    }
    map->size = size;
    return map;
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

int hash_map_add(struct ast *ast, char **farg)
{
    for (size_t k = 0; ast->ast_union.ast_simple_command.ass_word[k] != NULL;
         k++)
    {
        char *ass_word = strdup(ast->ast_union.ast_simple_command.ass_word[k]);
        char *value = get_value(ass_word);
        char *hash_value = expansion(strdup(value), farg);
        char *hash_key = get_key(ass_word);
        hash_map_insert(get_hm(), hash_key, hash_value, NULL);
    }
    return 1;
}

static bool str_equal(const char *str1, const char *str2)
{
    int ind = 0;
    char c1 = str1[0];
    char c2 = str2[0];
    while (c1 && c2)
    {
        if (c1 != c2)
        {
            return false;
        }
        ind += 1;
        c1 = str1[ind];
        c2 = str2[ind];
    }
    if (!c1 && !c2)
    {
        return true;
    }
    return false;
}

static struct pair_list *list_prepend(struct pair_list *list, char *value,
                                      char *key)
{
    struct pair_list *new = malloc(sizeof(struct pair_list));
    if (new == NULL)
    {
        return NULL;
    }
    new->value = value;
    new->next = list;
    new->key = key;
    return new;
}

static struct pair_list *is_in(struct pair_list *list, const char *key)
{
    while (list != NULL)
    {
        if (str_equal(list->key, key))
        {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

bool hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                     bool *updated)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
    size_t ind = hash(key) % hash_map->size;
    struct pair_list *list = is_in(hash_map->data[ind], key);
    if (list == NULL)
    {
        hash_map->data[ind] = list_prepend(hash_map->data[ind], value, (key));
        if (updated != NULL)
        {
            *updated = false;
        }
        if (hash_map->data[ind] == NULL)
        {
            return false;
        }
    }
    else
    {
        free(list->key);
        free(list->value);
        list->key = key;
        list->value = value;
        if (updated != NULL)
        {
            *updated = true;
        }
    }
    return true;
}

static void list_destroy(struct pair_list *list)
{
    if (list == NULL)
    {
        return;
    }
    free(list->key);
    free(list->value);
    list_destroy(list->next);
    free(list);
    return;
}

void hash_map_free(struct hash_map *hash_map)
{
    if (hash_map == NULL)
    {
        return;
    }
    for (size_t i = 0; i < hash_map->size; i++)
    {
        list_destroy(hash_map->data[i]);
    }
    free(hash_map->data);
    free(hash_map);
    return;
}

char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return NULL;
    }
    size_t ind = hash(key) % hash_map->size;
    struct pair_list *list = is_in(hash_map->data[ind], key);
    if (list == NULL)
    {
        return NULL;
    }
    return list->value;
}

static bool list_remove(struct pair_list *list, const char *key)
{
    while (list->next != NULL)
    {
        if (str_equal(list->next->key, key))
        {
            struct pair_list *cpy = list->next;
            list->next = list->next->next;
            free(cpy->key);
            free(cpy->value);
            free(cpy);
            return true;
        }
        list = list->next;
    }
    return false;
}

bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
    size_t ind = hash(key) % hash_map->size;
    struct pair_list *list = hash_map->data[ind];
    if (list == NULL)
    {
        return false;
    }
    if (str_equal(list->key, key))
    {
        hash_map->data[ind] = list->next;
        free(list->key);
        free(list->value);
        free(list);
        return true;
    }
    return list_remove(list, key);
}
