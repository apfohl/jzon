#include <stdlib.h>
#include "jzon.h"
#include "parser.h"
#include "lexer.h"

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *, int, const char *, struct jzon *);
void ParseFree(void *, void (*freeProc)(void *));

void object_free(struct jzon_object *object);
void array_free(struct jzon_array *array);

struct jzon *jzon_parse(const char *data)
{
    struct jzon *jzon = calloc(1, sizeof(struct jzon));

    yyscan_t scanner;
    yylex_init(&scanner);

    YY_BUFFER_STATE bufferState = yy_scan_string(data, scanner);

    void *parser = ParseAlloc(malloc);

    int token;
    while ((token = yylex(scanner))) {
        if (token == -1) {
            break;
        }

        char *text = strdup(yyget_text(scanner));
        Parse(parser, token, text, jzon);
    }
    Parse(parser, 0, NULL, jzon);

    ParseFree(parser, free);

    yy_delete_buffer(bufferState, scanner);

    yylex_destroy(scanner);

    if (jzon->type == JZON_ERROR) {
        free(jzon);

        return NULL;
    }

    return jzon;
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

int jzon_is_object(struct jzon *jzon);

int jzon_is_array(struct jzon *jzon);
