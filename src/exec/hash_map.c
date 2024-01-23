#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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
                                      const char *key)
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

static void print_list(struct pair_list *list)
{
    if (list == NULL)
    {
        return;
    }
    printf("%s: %s", list->key, list->value);
    list = list->next;
    while (list != NULL)
    {
        printf(", %s: %s", list->key, list->value);
        list = list->next;
    }
    printf("\n");
    return;
}

void hash_map_dump(struct hash_map *hash_map)
{
    if (hash_map == NULL)
    {
        return;
    }
    for (size_t i = 0; i < hash_map->size; i++)
    {
        print_list(hash_map->data[i]);
    }
    return;
}

const char *hash_map_get(const struct hash_map *hash_map, const char *key)
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
        free(list);
        return true;
    }
    return list_remove(list, key);
}
