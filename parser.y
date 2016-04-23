%include {
    #include <stdlib.h>
    #include <assert.h>
    #include <string.h>
    #include <errno.h>
    #include "jzon.h"

    struct pair {
        char *string;
        struct jzon *value;
    };

    struct members {
        int size;
        struct pair **pairs;
    };

    struct jzon_object *object_create(int capacity);
    void jzon_free(struct jzon *value);
    struct jzon_array *array_create(int capacity);
    struct jzon_object *object_put(struct jzon_object *object, const char *key,
        struct jzon *value);
    void set_error(enum jzon_error_type error, const char *fmt, ...);
    void *jzon_calloc(size_t count, size_t size);
    void *jzon_realloc(void *ptr, size_t size);
    void object_free(struct jzon_object *object);
    void array_free(struct jzon_array *array);

    char *string_remove_quotes(const char *data)
    {
        if (!data) {
            set_error(
                JZONE_INVALID_INPUT,
                "string_remove_quotes: %s",
                "No input string!"
            );
            return NULL;
        }

        int size = strlen(data);
        if (size <= 2) {
            set_error(
                JZONE_INVALID_INPUT,
                "string_remove_quotes: %s",
                "String is to short!"
            );
            return NULL;
        }

        char *str = jzon_calloc(size - 2 + 1, sizeof(char));
        if (!str) {
            return NULL;
        }

        strncpy(str, data + 1, size - 2);
        str[size - 2] = '\0';

        return str;
    }
}

%syntax_error {
    jzon->type = JZON_ERROR;
}

%token_destructor {
    free($$);
    (void)jzon;
}

%extra_argument { struct jzon *jzon }
%token_type { char * }
%default_type { void * }
%start_symbol start

%type object { struct jzon_object * }
%type array { struct jzon_array * }
%type value { struct jzon * }
%type pair { struct pair * }
%type members { struct members * }
%type elements { struct jzon_array * }

%destructor object {
    object_free($$);
}

%destructor array {
    array_free($$);
}

%destructor value {
    jzon_free($$);
}

%destructor pair {
    free($$->string);
    jzon_free($$->value);
    free($$);
}

%destructor members {
    for (int i = 0; i < $$->size; i++) {
        free($$->pairs[i]->string);
        jzon_free($$->pairs[i]->value);
        free($$->pairs[i]);
    }

    free($$->pairs);
    free($$);
}

%destructor elements {
    for (int i = 0; i < $$->capacity; i++) {
        jzon_free($$->elements[i]);
    }

    free($$->elements);
    free($$);
}

start ::= object(O). {
    jzon->type = JZON_OBJECT;
    jzon->object = O;
}
start ::= array(A). {
    jzon->type = JZON_ARRAY;
    jzon->array = A;
}
start ::= error. {
    jzon->type = JZON_ERROR;
}

object(O) ::= LBRACE RBRACE. {
    O = NULL;
}
object(O) ::= LBRACE members(M) RBRACE. {
    O = object_put(NULL, M->pairs[0]->string, M->pairs[0]->value);
    if (O) {
        free(M->pairs[0]->string);
        free(M->pairs[0]);

        for (int i = 1; i < M->size; i++) {
            O = object_put(O, M->pairs[i]->string, M->pairs[i]->value);
            free(M->pairs[i]->string);
            free(M->pairs[i]);
        }

        free(M->pairs);
        free(M);
    }
}

members(M) ::= pair(P). {
    M = jzon_calloc(1, sizeof(struct members));
    if (M) {
        M->size = 1;
        M->pairs = jzon_calloc(M->size, sizeof(struct pair *));
        if (M->pairs) {
            M->pairs[M->size - 1] = P;
        }
    }
}
members(M) ::= members(M_IN) COMMA pair(P). {
    M = M_IN;
    M->size++;
    M->pairs = jzon_realloc(M->pairs, M->size * sizeof(struct pair *));
    if (M->pairs) {
        M->pairs[M->size - 1] = P;
    }
}

pair(P) ::= STRING(S) COLON value(V). {
    P = jzon_calloc(1, sizeof(struct pair));
    if (P) {
        P->string = string_remove_quotes(S);
        P->value = V;
    }
    free(S);
}

array(A) ::= LBRACKET RBRACKET. {
    A = jzon_calloc(1, sizeof(struct jzon_array));
    if (A) {
        A->capacity = 0;
        A->elements = NULL;
    }
}
array(A) ::= LBRACKET elements(E) RBRACKET. {
    A = E;
}

elements(E) ::= value(V). {
    E = jzon_calloc(1, sizeof(struct jzon_array));
    if (E) {
        E->capacity = 1;
        E->elements = jzon_calloc(E->capacity, sizeof(struct jzon_value *));
        if (E->elements) {
            E->elements[E->capacity - 1] = V;
        }
    }
}
elements(E) ::= elements(E_IN) COMMA value(V). {
    E = E_IN;
    E->capacity++;
    E->elements =
        jzon_realloc(E->elements, E->capacity * sizeof(struct jzon_value *));
    if (E->elements) {
        E->elements[E->capacity - 1] = V;
    }
}

value(V) ::= object(O). {
    V = jzon_calloc(1, sizeof(struct jzon));
    if (V) {
        V->type = JZON_OBJECT;
        V->object = O;
    }
}
value(V) ::= array(A). {
    V = jzon_calloc(1, sizeof(struct jzon));
    if (V) {
        V->type = JZON_ARRAY;
        V->array = A;
    }
}
value(V) ::= NUMBER(N). {
    V = jzon_calloc(1, sizeof(struct jzon));
    if (V) {
        V->type = JZON_NUMBER;
        V->number = atof(N);
    }
    free(N);
}
value(V) ::= STRING(S). {
    V = jzon_calloc(1, sizeof(struct jzon));
    if (V) {
        V->type = JZON_STRING;
        V->string = string_remove_quotes(S);
    }
    free(S);
}
value(V) ::= TRUE. {
    V = jzon_calloc(1, sizeof(struct jzon));
    if (V) {
        V->type = JZON_BOOLEAN;
        V->boolean = 1;
    }
}
value(V) ::= FALSE. {
    V = jzon_calloc(1, sizeof(struct jzon));
    if (V) {
        V->type = JZON_BOOLEAN;
        V->boolean = 0;
    }
}
value(V) ::= NUL. {
    V = jzon_calloc(1, sizeof(struct jzon));
    if (V) {
        V->type = JZON_NULL;
    }
}
