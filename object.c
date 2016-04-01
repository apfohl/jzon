#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "jzon.h"

void jzon_free(struct jzon *value);
void set_error(enum jzon_error_type error, const char *fmt, ...);
void *jzon_calloc(size_t count, size_t size);
char *jzon_strdup(const char *s1);

static int max(int lhs, int rhs)
{
    return lhs > rhs ? lhs : rhs;
}

static int height(struct jzon_object *object)
{
    if (!object) {
        return 0;
    }

    return object->height;
}

static int balance(struct jzon_object *object)
{
    if (!object) {
        return 0;
    }

    return height(object->left) - height(object->right);
}

static struct jzon_object *rotate_left(struct jzon_object *object)
{
    struct jzon_object *right = object->right;
    struct jzon_object *left = right->left;

    right->left = object;
    object->right = left;

    object->height = max(height(object->left), height(object->right)) + 1;
    right->height = max(height(right->left), height(right->right)) + 1;

    return right;
}

static struct jzon_object *rotate_right(struct jzon_object *object)
{
    struct jzon_object *left = object->left;
    struct jzon_object *right = left->right;

    left->right = object;
    object->left = right;

    object->height = max(height(object->left), height(object->right)) + 1;
    left->height = max(height(left->left), height(left->right)) + 1;

    return left;
}

static struct jzon_object *object_create(const char *key, struct jzon *value)
{
    if (!key) {
        set_error(JZONE_INVAL, "object_create: %s", "No key is given");
        return NULL;
    }

    if (!value) {
        set_error(JZONE_INVAL, "object_create: %s", "No value is given");
        return NULL;
    }

    struct jzon_object *object = jzon_calloc(1, sizeof(struct jzon_object));
    if (!object) {
        return NULL;
    }

    object->key = jzon_strdup(key);
    if (!object->key) {
        free(object);
        return NULL;
    }

    object->value = value;
    object->height = 1;
    object->left = NULL;
    object->right = NULL;

    return object;
}

struct jzon_object *object_put(struct jzon_object *object, const char *key,
                               struct jzon *value)
{
    if (!key) {
        set_error(JZONE_INVAL, "object_create: %s", "No key is given");
        return NULL;
    }

    if (!value) {
        set_error(JZONE_INVAL, "object_create: %s", "No value is given");
        return NULL;
    }

    if (!object) {
        return object_create(key, value);
    }

    int cmp = strcmp(key, object->key);
    if (cmp == 0) {
        object->value = value;
        return object;
    } else if (cmp < 0) {
        object->left = object_put(object->left, key, value);
    } else {
        object->right = object_put(object->right, key, value);
    }

    object->height = max(height(object->left), height(object->right)) + 1;

    int b = balance(object);

    /* left left */
    if (b > 1 && strcmp(key, object->left->key) < 0) {
        return rotate_right(object);
    }

    /* left right */
    if (b > 1 && strcmp(key, object->left->key) > 0) {
        object->left = rotate_left(object->left);
        return rotate_right(object);
    }

    /* right right */
    if (b < -1 && strcmp(key, object->right->key) > 0) {
        return rotate_left(object);
    }

    /* right left */
    if (b < -1 && strcmp(key, object->right->key) < 0) {
        object->right = rotate_right(object->right);
        return rotate_left(object);
    }

    return object;
}

struct jzon *object_get(struct jzon_object *object, const char *key)
{
    if (!object) {
        set_error(JZONE_INVAL, "object_get: %s", "No object is given");
        return NULL;
    }

    if (!key) {
        set_error(JZONE_INVAL, "object_get: %s", "No key is given");
        return NULL;
    }

    struct jzon_object *temp = object;

    while (temp) {
        int cmp = strcmp(key, temp->key);
        if (cmp == 0) {
            return temp->value;
        } else if (cmp < 0) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }

    return NULL;
}

void object_free(struct jzon_object *object)
{
    if (object) {
        if (object->left) {
            object_free(object->left);
        }

        if (object->right) {
            object_free(object->right);
        }

        free(object->key);
        jzon_free(object->value);

        free(object);
    }
}
