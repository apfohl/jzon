#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "jzon.h"

void jzon_free(struct jzon *value);
void set_error(enum jzon_error_type error, const char *fmt, ...);
void *jzon_calloc(size_t count, size_t size);

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
    if (capacity <= 0) {
        set_error(JZONE_INVAL, "object_create: %s", "Invalid capacity");
        return NULL;
    }

    struct jzon_object *object = jzon_calloc(1, sizeof(struct jzon_object));
    if (!object) {
        return NULL;
    }

    object->capacity = capacity;
    object->size = 0;
    object->members = jzon_calloc(object->capacity, sizeof(struct jzon_member));
    if (!object->members) {
        free(object);
        return NULL;
    }

    for (int i = 0; i < object->capacity; i++) {
        object->members[i].key = NULL;
        object->members[i].value = NULL;
    }

    return object;
}

int object_put(struct jzon_object *object, const char *key,
               struct jzon *value)
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
        object->members[position].key = jzon_calloc(strlen(key) + 1, sizeof(char));
        if (!object->members[position].key) {
            return -1;
        }

        strcpy(object->members[position].key, key);
    }

    object->members[position].value = value;

    return 0;
}

struct jzon *object_get(struct jzon_object *object, const char *key)
{
    struct jzon *value = NULL;

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
            jzon_free(object->members[i].value);
        }
    }

    free(object->members);
    free(object);
}
