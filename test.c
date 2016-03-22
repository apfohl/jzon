#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jzon.h"

int main(int argc, const char **argv)
{
    struct jzon *jzon = jzon_parse("{\"number\": -1.3E1}");

    if (jzon_is_object(jzon)) {
        struct jzon *number = jzon_object_get(jzon, "number");

        if (jzon_is_number(number)) {
            fprintf(stdout, "%f\n", number->number);
        }
    }

    jzon_free(jzon);

    return 0;
}
