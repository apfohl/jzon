#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "jzon.h"

void jzon_free(struct jzon *value);
void set_error(enum jzon_error_type *error, enum jzon_error_type error_type);
void *jzon_calloc(size_t count, size_t size, enum jzon_error_type *error);
char *jzon_strdup(const char *s1, enum jzon_error_type *error);

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

static struct jzon_object *object_create(const char *key, struct jzon *value,
        enum jzon_error_type *error)
{
    if (!key) {
        set_error(error, JZONE_INVALID_INPUT);
        return NULL;
    }

    if (!value) {
        set_error(error, JZONE_INVALID_INPUT);
        return NULL;
    }

    enum jzon_error_type calloc_error;
    struct jzon_object *object = jzon_calloc(1, sizeof(struct jzon_object),
                                 &calloc_error);
    if (calloc_error != JZONE_NONE) {
        set_error(error, calloc_error);
        return NULL;
    }

    enum jzon_error_type strdup_error;
    object->key = jzon_strdup(key, &strdup_error);
    if (strdup_error != JZONE_NONE) {
        free(object);
        set_error(error, strdup_error);
        return NULL;
    }

    object->value = value;
    object->height = 1;
    object->left = NULL;
    object->right = NULL;

    set_error(error, JZONE_NONE);
    return object;
}

struct jzon_object *object_put(struct jzon_object *object, const char *key,
                               struct jzon *value, enum jzon_error_type *error)
{
    if (!key) {
        set_error(error, JZONE_INVALID_INPUT);
        return NULL;
    }

    if (!value) {
        set_error(error, JZONE_INVALID_INPUT);
        return NULL;
    }

    if (!object) {
        return object_create(key, value, error);
    }

    enum jzon_error_type object_put_error = JZONE_NONE;
    int cmp = strcmp(key, object->key);
    if (cmp == 0) {
        jzon_free(object->value);
        object->value = value;
        return object;
    } else if (cmp < 0) {
        object->left = object_put(object->left, key, value, &object_put_error);
    } else {
        object->right = object_put(object->right, key, value, &object_put_error);
    }

    if (object_put_error != JZONE_NONE) {
        set_error(error, object_put_error);
        return NULL;
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

    set_error(error, JZONE_NONE);
    return object;
}

struct jzon *object_get(struct jzon_object *object, const char *key,
                        enum jzon_error_type *error)
{
    if (!object) {
        set_error(error, JZONE_INVALID_INPUT);
        return NULL;
    }

    if (!key) {
        set_error(error, JZONE_INVALID_INPUT);
        return NULL;
    }

    struct jzon_object *temp = object;

    while (temp) {
        int cmp = strcmp(key, temp->key);
        if (cmp == 0) {
            set_error(error, JZONE_NONE);
            return temp->value;
        } else if (cmp < 0) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }

    set_error(error, JZONE_NO_ENTRY);
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
