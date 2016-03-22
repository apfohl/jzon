#include <stdlib.h>
#include "jzon.h"

void value_free(struct jzon *value);

void array_free(struct jzon_array *array)
{
    for (int i = 0; i < array->capacity; i++) {
        value_free(array->elements[i]);
    }

    free(array->elements);
    free(array);
}
