#include <speck.h>
#include <jzon.h>

void spec_parse_empty_object(void)
{
    struct jzon *jzon = jzon_parse("{}");

    sp_assert(jzon != NULL);
    sp_assert(jzon_error.error == JZONE_NONE);

    jzon_free(jzon);
}

void spec_parse_object(void)
{
    struct jzon *jzon =
        jzon_parse("{\"object\": {\"number\": 42}, \"array\": [1, 2, 3]}");

    sp_assert(jzon != NULL);
    sp_assert(jzon_error.error == JZONE_NONE);

    sp_assert(
        jzon_get_number(
            jzon_object_get(jzon_object_get(jzon, "object"), "number")
        ) == 42
    );
    sp_assert(
        jzon_get_number(
            jzon_array_get(jzon_object_get(jzon, "array"), 0)
        ) == 1
    );
    sp_assert(
        jzon_get_number(
            jzon_array_get(jzon_object_get(jzon, "array"), 1)
        ) == 2
    );
    sp_assert(
        jzon_get_number(
            jzon_array_get(jzon_object_get(jzon, "array"), 2)
        ) == 3
    );

    jzon_free(jzon);
}

void spec_parse_empty_array(void)
{
    struct jzon *jzon = jzon_parse("[]");

    sp_assert(jzon != NULL);
    sp_assert(jzon_error.error == JZONE_NONE);

    jzon_free(jzon);
}

void spec_parse_error(void)
{
    struct jzon *jzon = jzon_parse("[");

    sp_assert(jzon == NULL);
    sp_assert(jzon_error.error != JZONE_NONE);
}
