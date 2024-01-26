#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "ast/ast.h"
#include <stdbool.h>
#include <stddef.h>

struct pair_list
{
    char *key;
    char *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};

size_t hash(const char *str);
struct hash_map *hash_map_init(size_t size);
bool hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                     bool *updated);
void hash_map_free(struct hash_map *hash_map);
void hash_map_dump(struct hash_map *hash_map);
char *hash_map_get(const struct hash_map *hash_map, const char *key);
bool hash_map_remove(struct hash_map *hash_map, const char *key);
struct hash_map *get_hm(void);
int hash_map_add(struct ast *ast);

#endif /* ! HASH_MAP_H */
