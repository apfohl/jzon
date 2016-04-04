#include <speck.h>
#include <jzon.h>
#include <sys/stat.h>
#include <stdio.h>

void spec_access_non_existent_key(void)
{
    struct jzon *jzon = jzon_parse("{\"ip2\": \"127.0.0.1\"}");

    struct jzon *value = jzon_object_get(jzon, "ip");
    sp_assert(value == NULL);

    jzon_free(jzon);
}

void spec_simple_object(void)
{
    struct jzon *jzon = jzon_parse("{\"number\": 42}");
    sp_assert(jzon != NULL);
    sp_assert(jzon->type == JZON_OBJECT);
    sp_assert(jzon->object != NULL);

    jzon_free(jzon);
}

void spec_three_members(void)
{
    struct jzon *jzon = jzon_parse(
        "{\"type\": null, \"properties\": null, \"geometry\": null}"
    );
    sp_assert(jzon != NULL);

    jzon_free(jzon);
}
