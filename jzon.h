#ifndef JZON_H
#define JZON_H

enum jzon_value_type {
    VT_NUMBER,
    VT_OBJECT,
    VT_ARRAY,
    VT_STRING,
    VT_BOOLEAN,
    VT_NULL
};

struct jzon_value {
    enum jzon_value_type type;
    union {
        double number;
        struct jzon_object *object;
        struct jzon_array *array;
        char *string;
        unsigned int boolean;
    };
};

struct jzon_member {
    char *key;
    struct jzon_value *value;
};

struct jzon_object {
    int capacity;
    int size;
    struct jzon_member *members;
};

struct jzon_array {
    int capacity;
    struct jzon_value **elements;
};

enum jzon_type {
    J_OBJECT,
    J_ARRAY,
    J_ERROR
};

struct jzon {
    enum jzon_type type;
    union {
        struct jzon_object *object;
        struct jzon_array *array;
    };
};

struct jzon *jzon_parse(const char *data);

void jzon_free(struct jzon *jzon);

struct jzon_value *object_get(struct jzon_object *object, const char *key);

#endif
