#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jzon.h"

int main(int argc, const char **argv)
{
    struct jzon *jzon = NULL;

    if (argc > 1) {
        jzon = jzon_parse(argv[1]);
    } else {
        jzon = jzon_parse("{\"number\": -1.3E1}");
    }

    if (jzon) {
        fprintf(stdout, "number: %f\n", object_get(jzon->object, "number")->number);

        jzon_free(jzon);
    }

    return 0;
}
