#ifndef JZON_H
#define JZON_H

enum jzon_error_type {
    JZONE_NONE,
    JZONE_OUT_OF_MEMORY,
    JZONE_INVALID_INPUT,
    JZONE_LEXER_ERROR,
    JZONE_PARSER_ERROR,
    JZONE_NO_ENTRY,
    JZONE_ARRAY_OUT_OF_BOUNDS
};

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

struct jzon_object {
    char *key;
    struct jzon *value;
    int height;
    struct jzon_object *left;
    struct jzon_object *right;
};

struct jzon_array {
    int capacity;
    struct jzon **elements;
};

const char *get_error_string(enum jzon_error_type error);

struct jzon *jzon_parse(const char *data, enum jzon_error_type *error);

void jzon_free(struct jzon *jzon);

int jzon_is_object(struct jzon *jzon, enum jzon_error_type *error);

struct jzon *jzon_object_get(struct jzon *jzon, const char *key,
                             enum jzon_error_type *error);

int jzon_is_array(struct jzon *jzon, enum jzon_error_type *error);

struct jzon *jzon_array_get(struct jzon *jzon, int index,
                            enum jzon_error_type *error);

int jzon_array_size(struct jzon *jzon, enum jzon_error_type *error);

int jzon_is_number(struct jzon *jzon, enum jzon_error_type *error);

double jzon_get_number(struct jzon *jzon, enum jzon_error_type *error);

int jzon_is_string(struct jzon *jzon, enum jzon_error_type *error);

char *jzon_get_string(struct jzon *jzon, enum jzon_error_type *error);

int jzon_is_boolean(struct jzon *jzon, enum jzon_error_type *error);

int jzon_get_boolean(struct jzon *jzon, enum jzon_error_type *error);

int jzon_is_null(struct jzon *jzon, enum jzon_error_type *error);

#endif
