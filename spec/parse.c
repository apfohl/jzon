#include <speck.h>
#include <jzon.h>

void spec_parse_empty_object(void)
{
    struct jzon *jzon = jzon_parse("{}", NULL);

    sp_assert(jzon != NULL);

    jzon_free(jzon);
}

void spec_parse_object(void)
{
    struct jzon *jzon =
        jzon_parse("{\"object\": {\"number\": 42}, \"array\": [1, 2, 3]}", NULL);

    sp_assert(jzon != NULL);

    sp_assert(
        jzon_get_number(
            jzon_object_get(
                jzon_object_get(jzon, "object", NULL), "number", NULL
            ), NULL
        ) == 42
    );
    sp_assert(
        jzon_get_number(
            jzon_array_get(jzon_object_get(jzon, "array", NULL), 0, NULL), NULL
        ) == 1
    );
    sp_assert(
        jzon_get_number(
            jzon_array_get(jzon_object_get(jzon, "array", NULL), 1, NULL), NULL
        ) == 2
    );
    sp_assert(
        jzon_get_number(
            jzon_array_get(jzon_object_get(jzon, "array", NULL), 2, NULL), NULL
        ) == 3
    );

    jzon_free(jzon);
}

void spec_parse_empty_array(void)
{
    struct jzon *jzon = jzon_parse("[]", NULL);

    sp_assert(jzon != NULL);

    jzon_free(jzon);
}

void spec_parse_error(void)
{
    struct jzon *jzon = jzon_parse("[", NULL);

    sp_assert(jzon == NULL);
}
