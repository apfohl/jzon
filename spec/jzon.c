#include <speck.h>
#include <jzon.h>
#include <string.h>

void spec_getter(void)
{
    const char *str =
        "{\"number\": 42, \"string\": \"This is a test.\", \"boolean\": true}";

    struct jzon *jzon = jzon_parse(str, NULL);

    sp_assert(
        jzon_get_number(jzon_object_get(jzon, "number", NULL), NULL) == 42
    );

    sp_assert(
        strcmp(
            jzon_get_string(jzon_object_get(jzon, "string", NULL), NULL),
            "This is a test."
        ) == 0
    );

    sp_assert(
        jzon_get_boolean(jzon_object_get(jzon, "boolean", NULL), NULL) != 0
    );

    jzon_free(jzon);
}
