#include <speck.h>
#include <jzon.h>

void spec_simple_object(void)
{
    struct jzon *jzon = jzon_parse("{\"number\": 42}");
    sp_assert(jzon != NULL);
    sp_assert(jzon->type == JZON_OBJECT);
    sp_assert(jzon->object != NULL);

    jzon_free(jzon);
}
