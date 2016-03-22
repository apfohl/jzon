#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jzon.h"

int main(int argc, const char **argv)
{
    struct jzon *object = jzon_parse("{\"number\": -1.3E1}");

    if (jzon_is_object(object)) {
        struct jzon *number = jzon_object_get(object, "number");

        if (jzon_is_number(number)) {
            fprintf(stdout, "%f\n", number->number);
        }
    }

    jzon_free(object);

    puts("");

    struct jzon *array = jzon_parse("[1, 2, 3]");

    if (jzon_is_array(array)) {
        for (int i = 0; i < jzon_array_size(array); i++) {
            struct jzon *number = jzon_array_get(array, i);

            if (jzon_is_number(number)) {
                fprintf(stdout, "%f\n", number->number);
            }
        }
    }

    jzon_free(array);

    return 0;
}
