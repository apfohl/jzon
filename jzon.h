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
        double number;
        struct jzon_object *object;
        struct jzon_array *array;
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

#endif
