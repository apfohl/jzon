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
        jzon = jzon_parse("[\"foo\", \"bar\"");
    }

    if (jzon) {
        fprintf(stdout, "jzon: %p\n", jzon);

        jzon_free(jzon);
    }

    return 0;
}
