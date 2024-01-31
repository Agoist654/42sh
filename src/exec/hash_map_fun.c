#define _XOPEN_SOURCE 500
#include "hash_map_fun.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHMAP_SIZE 8

static struct hash_map_fun *hm = NULL;

static struct hash_map_fun *hash_map_fun_init(size_t size)
{
    struct hash_map_fun *map = malloc(sizeof(struct hash_map_fun));
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

struct hash_map_fun *get_fun_hm(void)
{
    if (hm == NULL)
        hm = hash_map_fun_init(HASHMAP_SIZE);
    return hm;
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

static struct pair_list_fun *list_prepend(struct pair_list_fun *list, struct ast *value,
                                      char *key)
{
    struct pair_list_fun *new = malloc(sizeof(struct pair_list_fun));
    if (new == NULL)
    {
        return NULL;
    }
    new->value = value;
    new->next = list;
    new->key = key;
    return new;
}

static struct pair_list_fun *is_in(struct pair_list_fun *list, const char *key)
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

bool hash_map_fun_insert(struct hash_map_fun *hash_map, char *key, struct ast *value,
                     bool *updated)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
    size_t ind = hash(key) % hash_map->size;
    struct pair_list_fun *list = is_in(hash_map->data[ind], key);
    if (list == NULL)
    {
        hash_map->data[ind] = list_prepend(hash_map->data[ind], value, key);
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
        if (list->value != NULL)
            list->value->ftable->destroy(list->value);
        list->key = key;
        list->value = value;
        if (updated != NULL)
        {
            *updated = true;
        }
    }
    return true;
}

static void list_destroy(struct pair_list_fun *list)
{
    if (list == NULL)
    {
        return;
    }
    free(list->key);
    if (list->value != NULL)
        list->value->ftable->destroy(list->value);
    list_destroy(list->next);
    free(list);
    return;
}

void hash_map_fun_free(struct hash_map_fun *hash_map)
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

struct ast *hash_map_fun_get(const struct hash_map_fun *hash_map, const char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return NULL;
    }
    size_t ind = hash(key) % hash_map->size;
    struct pair_list_fun *list = is_in(hash_map->data[ind], key);
    if (list == NULL)
    {
        return NULL;
    }
    return list->value;
}

static bool list_remove(struct pair_list_fun *list, const char *key)
{
    while (list->next != NULL)
    {
        if (str_equal(list->next->key, key))
        {
            struct pair_list_fun *cpy = list->next;
            list->next = list->next->next;
            free(cpy->key);
            if (cpy->value != NULL)
                cpy->value->ftable->destroy(cpy->value);
            free(cpy);
            return true;
        }
        list = list->next;
    }
    return false;
}

bool hash_map_fun_remove(struct hash_map_fun *hash_map, const char *key)
{
    if (hash_map == NULL || hash_map->size == 0)
    {
        return false;
    }
    size_t ind = hash(key) % hash_map->size;
    struct pair_list_fun *list = hash_map->data[ind];
    if (list == NULL)
    {
        return false;
    }
    if (str_equal(list->key, key))
    {
        hash_map->data[ind] = list->next;
        free(list->key);
        if (list->value != NULL)
            list->value->ftable->destroy(list->value);
        free(list);
        return true;
    }
    return list_remove(list, key);
}
