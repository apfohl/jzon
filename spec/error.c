#include <speck.h>
#include <jzon.h>

void spec_simple_error(void)
{
    enum jzon_error_type error;

    struct jzon *jzon = jzon_parse("[1, 2, 3", &error);
    sp_assert(jzon == NULL);
    sp_assert(error != JZONE_NONE);

    jzon_free(jzon);
}
