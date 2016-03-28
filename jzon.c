#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include "jzon.h"
#include "parser.h"
#include "lexer.h"

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *, int, const char *, struct jzon *);
void ParseFree(void *, void (*freeProc)(void *));

struct jzon *object_get(struct jzon_object *, const char *);
void object_free(struct jzon_object *);
void array_free(struct jzon_array *);

struct jzon_error jzon_error = {
    .error = JZONE_NONE,
    .msg = ""
};

void set_error(enum jzon_error_type error, const char *fmt, ...)
{
    jzon_error.error = error;

    va_list args;
    va_start(args, fmt);
    (void)vsnprintf(jzon_error.msg, 2047, fmt, args);
    va_end(args);
}

void *jzon_calloc(size_t count, size_t size)
{
    void *mem = calloc(count, size);
    if (!mem) {
        set_error(JZONE_OOM, "calloc: %s", strerror(errno));
    }

    return mem;
}

void *jzon_realloc(void *ptr, size_t size)
{
    void *mem = realloc(ptr, size);
    if (!mem) {
        set_error(JZONE_OOM, "realloc: %s", strerror(errno));
    }

    return mem;
}

char *jzon_strdup(const char *s1)
{
    char *str = strdup(s1);
    if (!str) {
        set_error(JZONE_OOM, "strdup: %s", strerror(errno));
    }

    return str;
}

struct jzon *jzon_parse(const char *data)
{
    jzon_error.error = JZONE_NONE;
    jzon_error.msg[0] = '\0';

    if (!data) {
        set_error(JZONE_INVAL, "jzon_parse: No input string!");
        return NULL;
    }

    struct jzon *jzon = jzon_calloc(1, sizeof(struct jzon));
    if (!jzon) {
        return NULL;
    }

    yyscan_t scanner;
    yylex_init(&scanner);

    YY_BUFFER_STATE buffer_state = yy_scan_string(data, scanner);
    if (!buffer_state) {
        set_error(JZONE_LEX, "yy_scan_string: %s", strerror(errno));
        goto error_1;
    }

    void *parser = ParseAlloc(malloc);
    if (!parser) {
        set_error(JZONE_PARSE, "ParseAlloc: %s", strerror(errno));
        goto error_2;
    }

    int token;
    while ((token = yylex(scanner))) {
        if (token == -1) {
            break;
        }

        char *text = strdup(yyget_text(scanner));
        if (!text) {
            goto error_3;
        }
        Parse(parser, token, text, jzon);

        if (jzon_error.error != JZONE_NONE) {
            goto error_3;
        }
    }
    Parse(parser, 0, NULL, jzon);

    ParseFree(parser, free);

    yy_delete_buffer(buffer_state, scanner);

    yylex_destroy(scanner);

    if (jzon->type == JZON_ERROR) {
        jzon_error.error = JZONE_PARSE;
        sprintf(jzon_error.msg, "jzon_parse: Parsing error");
        free(jzon);
        return NULL;
    }

    return jzon;

error_3:
    ParseFree(parser, free);

error_2:
    yy_delete_buffer(buffer_state, scanner);

error_1:
    yylex_destroy(scanner);
    free(jzon);

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

int jzon_is_object(struct jzon *jzon)
{
    return jzon->type == JZON_OBJECT;
}

int jzon_is_array(struct jzon *jzon)
{
    return jzon->type == JZON_ARRAY;
}

int jzon_is_number(struct jzon *jzon)
{
    return jzon->type == JZON_NUMBER;
}

int jzon_is_string(struct jzon *jzon)
{
    return jzon->type == JZON_STRING;
}

int jzon_is_boolean(struct jzon *jzon)
{
    return jzon->type == JZON_BOOLEAN;
}

int jzon_is_null(struct jzon *jzon)
{
    return jzon->type == JZON_NULL;
}

struct jzon *jzon_object_get(struct jzon *jzon, const char *key)
{
    if (jzon_is_object(jzon)) {
        return object_get(jzon->object, key);
    } else {
        return NULL;
    }
}

struct jzon *jzon_array_get(struct jzon *jzon, int index)
{
    if (jzon_is_array(jzon) &&
            index < jzon->array->capacity &&
            index >= 0) {
        return jzon->array->elements[index];
    } else {
        return NULL;
    }
}

int jzon_array_size(struct jzon *jzon)
{
    if (jzon_is_array(jzon)) {
        return jzon->array->capacity;
    } else {
        return -1;
    }
}
