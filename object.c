#include <string.h>
#include <stdlib.h>
#include "jzon.h"

void value_free(struct jzon_value *value);

unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

struct jzon_object *object_create(int capacity)
{
    struct jzon_object *object = calloc(1, sizeof(struct jzon_object));
    object->capacity = capacity;
    object->size = 0;
    object->members = calloc(object->capacity, sizeof(struct jzon_member));

    for (int i = 0; i < object->capacity; i++) {
        object->members[i].key = NULL;
        object->members[i].value = NULL;
    }

    return object;
}

int object_put(struct jzon_object *object, const char *key,
               struct jzon_value *value)
{
    if (!object || !key) {
        return -1;
    }

    unsigned long hashval = hash(key);
    unsigned long position;
    unsigned int reassign = 0;
    int i = 1;

    do {
        // Quadratic probing
        position = hashval + ((int)(0.5 * i)) + ((int)(0.5 * i * i));
        position %= object->capacity;
        i++;
    } while (
        object->members[position].key &&
        !(reassign = strcmp(object->members[position].key, key) == 0)
    );

    if (!reassign) {
        object->size++;
        object->members[position].key = calloc(strlen(key) + 1, sizeof(char));
        strcpy(object->members[position].key, key);
    }

    object->members[position].value = value;

    return 0;
}

struct jzon_value *object_get(struct jzon_object *object, const char *key)
{
    struct jzon_value *value = NULL;

    if (object && key) {
        unsigned long hashval = hash((char *)key);
        unsigned long position;
        int i = 1;

        do {
            // Quadratic probing
            position =
                hashval + ((int)(0.5 * i)) + ((int)(0.5 * i * i));
            position %= object->capacity;
            i++;
        } while (object->members[position].key &&
                 strcmp(object->members[position].key, key) != 0);

        if (object->members[position].key) {
            value = object->members[position].value;
        }
    }

    return value;
}

void object_free(struct jzon_object *object)
{
    if (!object) {
        return;
    }

    for (int i = 0; i < object->capacity; i++) {
        if (object->members[i].key) {
            free(object->members[i].key);
            value_free(object->members[i].value);
        }
    }

    free(object->members);
    free(object);
}
