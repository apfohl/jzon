#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jzon.h"

int main(int argc, const char **argv)
{
    struct jzon *jzon = jzon_parse("{\"number\": -1.3E1}");

    jzon_free(jzon);

    return 0;
}
