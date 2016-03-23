#ifndef JZON_H
#define JZON_H

enum jzon_type {
    JZON_NUMBER,
    JZON_OBJECT,
    JZON_ARRAY,
    JZON_STRING,
    JZON_BOOLEAN,
    JZON_NULL,
    JZON_ERROR
};

struct jzon {
    enum jzon_type type;
    union {
        struct jzon_object *object;
        struct jzon_array *array;
        double number;
        char *string;
        unsigned int boolean;
    };
};

struct jzon_member {
    char *key;
    struct jzon *value;
};

struct jzon_object {
    int capacity;
    int size;
    struct jzon_member *members;
};

struct jzon_array {
    int capacity;
    struct jzon **elements;
};

struct jzon *jzon_parse(const char *data);

void jzon_free(struct jzon *jzon);

int jzon_is_object(struct jzon *jzon);

int jzon_is_array(struct jzon *jzon);

int jzon_is_number(struct jzon *jzon);

int jzon_is_string(struct jzon *jzon);

int jzon_is_boolean(struct jzon *jzon);

int jzon_is_null(struct jzon *jzon);

struct jzon *jzon_object_get(struct jzon *jzon, const char *key);

struct jzon *jzon_array_get(struct jzon *jzon, int index);

int jzon_array_size(struct jzon *jzon);

#endif
