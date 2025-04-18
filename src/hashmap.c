#include "hashmap.h"
#include "string.h"
#include "stdlib.h"
#include "log.h"

hashmap init_hashmap(int capacity)
{
    hashmap map;
    map.map = calloc(capacity, sizeof(*map.map));
    map.capacity = capacity;
    return map;
}

size_t hash(const char* str, size_t capacity)
{
    unsigned long hash = 5381;
    char c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % capacity;
}

char* hget(struct hashmap* map, const char* key)
{
    size_t i = hash(key, map->capacity);
    struct hash_element* current = &(map->map[i]);
    while (current != NULL)
    {
        if (!current->key) return NULL;

        if (!strcmp(current->key, key))
        {
            return current->element;
        }
        current = current->next;
    }
    return NULL;
}

void hset(struct hashmap* map, const char* key, const char* element)
{
    size_t i = hash(key, map->capacity);
    struct hash_element* current = &(map->map[i]);
    while(current->key)
    {
        if (!strcmp(current->key, key))
        {
            free(current->element);
            break;
        }

        if (!current->next)
            current->next = calloc(sizeof(*current->next), 1);
        current = current->next;
    }
    current->key = strdup(key);
    current->element = strdup(element);
    if (!current->key || !current->element)
    {
        flog(LOG_ERROR, "shits fucked icl");
        exit(1);
    }
}
