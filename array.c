#include <stdlib.h>
#include "jzon.h"

void jzon_free(struct jzon *value);

void array_free(struct jzon_array *array)
{
    if (array) {
        for (int i = 0; i < array->capacity; i++) {
            jzon_free(array->elements[i]);
        }

        free(array->elements);
        free(array);
    }
}
