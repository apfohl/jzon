#include <stdlib.h>
#include "jzon.h"

void object_free(struct jzon_object *object);
void array_free(struct jzon_array *array);

void value_free(struct jzon *value)
{
    if (value) {
        switch (value->type) {
            case JZON_OBJECT:
                object_free(value->object);
                break;
            case JZON_ARRAY:
                array_free(value->array);
                break;
            case JZON_STRING:
                free(value->string);
                break;
            default:
                break;
        }

        free(value);
    }
}
