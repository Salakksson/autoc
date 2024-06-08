#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <stddef.h>

struct hash_element
{
    char* key;
    char* element;
    struct hash_element* next;
};

typedef struct hashmap
{
    struct hash_element* map;
    size_t capacity;
} hashmap;

hashmap init_hashmap(int capacity);

size_t hash(const char* str, size_t capacity);

char* hget(struct hashmap* map, const char* key);
void  hset(struct hashmap* map, const char* key, const char* element);

#endif
