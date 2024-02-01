#ifndef HASH_MAP_FUN_H
#define HASH_MAP_FUN_H

#include <stdbool.h>
#include <stddef.h>

#include "ast/ast.h"

struct pair_list_fun
{
    char *key;
    struct ast *value;
    struct pair_list_fun *next;
};

struct hash_map_fun
{
    struct pair_list_fun **data;
    size_t size;
};

size_t hash(const char *str);
bool hash_map_fun_insert(struct hash_map_fun *hash_map, char *key,
                         struct ast *value, bool *updated);
void hash_map_fun_free(struct hash_map_fun *hash_map);
struct ast *hash_map_fun_get(const struct hash_map_fun *hash_map,
                             const char *key);
bool hash_map_fun_remove(struct hash_map_fun *hash_map, const char *key);
struct hash_map_fun *get_fun_hm(void);

#endif /* ! HASH_MAP_FUN_H */
