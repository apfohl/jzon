#include <stdlib.h>
#include "jzon.h"

void object_free(struct jzon_object *object);

void value_free(struct jzon_value *value)
{
    if (value) {
        switch (value->type) {
            case VT_OBJECT:
                object_free(value->object);
                break;
            case VT_ARRAY:
                break;
            case VT_STRING:
                free(value->string);
                break;
            default:
                break;
        }

        free(value);
    }
}
