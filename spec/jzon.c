#include <speck.h>
#include <jzon.h>
#include <string.h>

void spec_getter(void)
{
    const char *str =
        "{\"number\": 42, \"string\": \"This is a test.\", \"boolean\": true}";

    struct jzon *jzon = jzon_parse(str);

    sp_assert(
        jzon_get_number(jzon_object_get(jzon, "number")) == 42
    );

    sp_assert(
        strcmp(
            jzon_get_string(jzon_object_get(jzon, "string")),
            "This is a test."
        ) == 0
    );

    sp_assert(
        jzon_get_boolean(jzon_object_get(jzon, "boolean")) != 0
    );

    jzon_free(jzon);
}
