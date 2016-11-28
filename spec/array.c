#include <speck.h>
#include <jzon.h>

void spec_simple_array(void)
{
    struct jzon *jzon = jzon_parse("[1, 2, 3]", NULL);
    sp_assert(jzon != NULL);
    sp_assert(jzon->type == JZON_ARRAY);
    sp_assert(jzon->array != NULL);
    sp_assert_equal_i(jzon->array->capacity, 3);
    sp_assert(jzon_get_number(jzon->array->elements[0], NULL) == 1.0);
    sp_assert(jzon_get_number(jzon->array->elements[1], NULL) == 2.0);
    sp_assert(jzon_get_number(jzon->array->elements[2], NULL) == 3.0);

    jzon_free(jzon);
}

void spec_string_with_escaped_quotes(void)
{
    const char *json_string = "[\"The Word \\\"Foo\\\" is so damn bar!\"]";

    struct jzon *jzon = jzon_parse(json_string, NULL);
    sp_assert(jzon != NULL);

    jzon_free(jzon);
}
