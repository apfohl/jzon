#include <stdlib.h>
#include <stdarg.h>
#include "jzon.h"
#include "parser.h"
#include "lexer.h"

struct parser_state {
    struct jzon *jzon;
    enum jzon_error_type error;
};

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *, int, const char *, struct parser_state *);
void ParseFree(void *, void (*freeProc)(void *));

struct jzon *object_get(struct jzon_object *object, const char *key,
    enum jzon_error_type *error);
void object_free(struct jzon_object *);
void array_free(struct jzon_array *);

void set_error(enum jzon_error_type *error, enum jzon_error_type error_type)
{
    if (error) {
        *error = error_type;
    }
}

const char *get_error_string(enum jzon_error_type error)
{
    char *error_string[] = {
        "JZONE_NONE",
        "JZONE_OUT_OF_MEMORY",
        "JZONE_INVALID_INPUT",
        "JZONE_LEXER_ERROR",
        "JZONE_PARSER_ERROR",
        "JZONE_NO_ENTRY",
        "JZONE_ARRAY_OUT_OF_BOUNDS"
    };

    return error_string[error];
}

void *jzon_calloc(size_t count, size_t size, enum jzon_error_type *error)
{
    void *mem = calloc(count, size);
    if (!mem) {
        set_error(error, JZONE_OUT_OF_MEMORY);
    } else {
        set_error(error, JZONE_NONE);
    }

    return mem;
}

void *jzon_realloc(void *ptr, size_t size, enum jzon_error_type *error)
{
    void *mem = realloc(ptr, size);
    if (!mem) {
        set_error(error, JZONE_OUT_OF_MEMORY);
    } else {
        set_error(error, JZONE_NONE);
    }

    return mem;
}

char *jzon_strdup(const char *s1, enum jzon_error_type *error)
{
    char *str = strdup(s1);
    if (!str) {
        set_error(error, JZONE_OUT_OF_MEMORY);
    } else {
        set_error(error, JZONE_NONE);
    }

    return str;
}

struct jzon *jzon_parse(const char *data, enum jzon_error_type *error)
{
    if (!data) {
        set_error(error, JZONE_INVALID_INPUT);
        return NULL;
    }

    enum jzon_error_type calloc_error;
    struct parser_state *state =
        jzon_calloc(1, sizeof(struct parser_state), &calloc_error);
    if (calloc_error != JZONE_NONE) {
        set_error(error, calloc_error);
        return NULL;
    }

    state->jzon = jzon_calloc(1, sizeof(struct jzon), &calloc_error);
    if (calloc_error != JZONE_NONE) {
        set_error(error, calloc_error);
        return NULL;
    }

    state->error = JZONE_NONE;

    yyscan_t scanner;
    yylex_init(&scanner);

    YY_BUFFER_STATE buffer_state = yy_scan_string(data, scanner);
    if (!buffer_state) {
        set_error(error, JZONE_LEXER_ERROR);
        goto error_1;
    }

    void *parser = ParseAlloc(malloc);
    if (!parser) {
        set_error(error, JZONE_PARSER_ERROR);
        goto error_2;
    }

    int token;
    while ((token = yylex(scanner))) {
        if (token == -1) {
            break;
        }

        enum jzon_error_type strdup_error;
        char *text = jzon_strdup(yyget_text(scanner), &strdup_error);
        if (strdup_error != JZONE_NONE) {
            set_error(error, strdup_error);
            goto error_3;
        }

        Parse(parser, token, text, state);

        if (state->error != JZONE_NONE) {
            set_error(error, state->error);
            goto error_3;
        }
    }
    Parse(parser, 0, NULL, state);

    ParseFree(parser, free);

    yy_delete_buffer(buffer_state, scanner);

    yylex_destroy(scanner);

    if (state->jzon->type == JZON_ERROR) {
        set_error(error, JZONE_PARSER_ERROR);
        free(state->jzon);
        free(state);
        return NULL;
    }

    set_error(error, JZONE_NONE);

    struct jzon *jzon = state->jzon;
    free(state);

    return jzon;

error_3:
    ParseFree(parser, free);

error_2:
    yy_delete_buffer(buffer_state, scanner);

error_1:
    yylex_destroy(scanner);
    free(state->jzon);
    free(state);

    return NULL;
}

void jzon_free(struct jzon *jzon)
{
    if (jzon) {
        switch (jzon->type) {
            case JZON_OBJECT:
                object_free(jzon->object);
                break;
            case JZON_ARRAY:
                array_free(jzon->array);
                break;
            case JZON_STRING:
                free(jzon->string);
                break;
            default:
                break;
        }

        free(jzon);
    }
}

int jzon_is_object(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->type == JZON_OBJECT;
}

struct jzon *jzon_object_get(struct jzon *jzon, const char *key,
                             enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return NULL;
    }

    enum jzon_error_type is_object_error;
    int is_object = jzon_is_object(jzon, &is_object_error);

    if (is_object_error != JZONE_NONE) {
        set_error(error, is_object_error);
        return NULL;
    }

    if (!is_object) {
        set_error(error, JZONE_NO_ENTRY);
        return NULL;
    }

    set_error(error, JZONE_NONE);

    return object_get(jzon->object, key, error);
}

int jzon_is_array(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->type == JZON_ARRAY;
}

struct jzon *jzon_array_get(struct jzon *jzon, int index,
                            enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return NULL;
    }

    enum jzon_error_type is_array_error;
    int is_array = jzon_is_array(jzon, &is_array_error);

    if (is_array_error != JZONE_NONE) {
        set_error(error, is_array_error);
        return NULL;
    }

    if (!is_array) {
        set_error(error, JZONE_NO_ENTRY);
        return NULL;
    }

    if (index < jzon->array->capacity && index >= 0) {
        set_error(error, JZONE_NONE);
        return jzon->array->elements[index];
    } else {
        set_error(error, JZONE_ARRAY_OUT_OF_BOUNDS);
        return NULL;
    }
}

int jzon_array_size(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    enum jzon_error_type is_array_error;
    int is_array = jzon_is_array(jzon, &is_array_error);

    if (is_array_error != JZONE_NONE) {
        set_error(error, is_array_error);
        return -1;
    }

    if (!is_array) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->array->capacity;
}

int jzon_is_number(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->type == JZON_NUMBER;
}

double jzon_get_number(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    enum jzon_error_type is_number_error;
    int is_number = jzon_is_number(jzon, &is_number_error);

    if (is_number_error != JZONE_NONE) {
        set_error(error, is_number_error);
        return -1;
    }

    if (!is_number) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->number;
}

int jzon_is_string(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->type == JZON_STRING;
}

char *jzon_get_string(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return NULL;
    }

    enum jzon_error_type is_string_error;
    int is_string = jzon_is_string(jzon, &is_string_error);

    if (is_string_error != JZONE_NONE) {
        set_error(error, is_string_error);
        return NULL;
    }

    if (!is_string) {
        set_error(error, JZONE_NO_ENTRY);
        return NULL;
    }

    set_error(error, JZONE_NONE);

    return jzon->string;
}

int jzon_is_boolean(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->type == JZON_BOOLEAN;
}

int jzon_get_boolean(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    enum jzon_error_type is_boolean_error;
    int is_boolean = jzon_is_boolean(jzon, &is_boolean_error);

    if (is_boolean_error != JZONE_NONE) {
        set_error(error, is_boolean_error);
        return -1;
    }

    if (!is_boolean) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->boolean;
}

int jzon_is_null(struct jzon *jzon, enum jzon_error_type *error)
{
    if (!jzon) {
        set_error(error, JZONE_NO_ENTRY);
        return -1;
    }

    set_error(error, JZONE_NONE);

    return jzon->type == JZON_NULL;
}
