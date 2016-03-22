%include {
    #include <stdlib.h>
    #include <assert.h>
    #include <stdio.h>
    #include <string.h>
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
    void value_free(struct jzon *value);
    struct jzon_array *array_create(int capacity);
    int object_put(struct jzon_object *object, const char *key,
                   struct jzon *value);

    char *string_remove_quotes(const char *data)
    {
        int size = strlen(data);
        char *str = calloc(size - 1, sizeof(char));

        strncpy(str, data + 1, size - 2);
        str[size - 2] = '\0';

        return str;
    }
}

%syntax_error {
    jzon->type = JZON_ERROR;
    fprintf(stderr, "Syntax error!\n");
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

%destructor members {
    for (int i = 0; i < $$->size; i++) {
        free($$->pairs[i]->string);
        value_free($$->pairs[i]->value);
        free($$->pairs[i]);
    }

    free($$->pairs);
    free($$);
}

%destructor elements {
    for (int i = 0; i < $$->capacity; i++) {
        value_free($$->elements[i]);
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
    fprintf(stderr, "error.\n");
}

object(O) ::= LBRACE RBRACE. {
    O = calloc(1, sizeof(struct jzon_object));
    O->capacity = 0;
    O->size = 0;
}
object(O) ::= LBRACE members(M) RBRACE. {
    O = object_create(M->size);

    for (int i = 0; i < M->size; i++) {
        object_put(O, M->pairs[i]->string, M->pairs[i]->value);
        free(M->pairs[i]->string);
        free(M->pairs[i]);
    }

    free(M->pairs);
    free(M);
}

members(M) ::= pair(P). {
    M = calloc(1, sizeof(struct members));
    M->size = 1;
    M->pairs = calloc(M->size, sizeof(struct pair *));
    M->pairs[M->size - 1] = P;
}
members(M) ::= members(M_IN) COMMA pair(P). {
    M = M_IN;
    M->size++;
    M->pairs = realloc(M->pairs, M->size * sizeof(struct pair *));
    M->pairs[M->size - 1] = P;
}

pair(P) ::= STRING(S) COLON value(V). {
    P = calloc(1, sizeof(struct pair));
    P->string = string_remove_quotes(S);
    P->value = V;
    free(S);
}

array(A) ::= LBRACKET RBRACKET. {
    A = calloc(1, sizeof(struct jzon_array));
    A->capacity = 0;
    A->elements = NULL;
}
array(A) ::= LBRACKET elements(E) RBRACKET. {
    A = E;
}

elements(E) ::= value(V). {
    E = calloc(1, sizeof(struct jzon_array));
    E->capacity = 1;
    E->elements = calloc(E->capacity, sizeof(struct jzon_value *));
    E->elements[E->capacity - 1] = V;
}
elements(E) ::= elements(E_IN) COMMA value(V). {
    E = E_IN;
    E->capacity++;
    E->elements =
        realloc(E->elements, E->capacity * sizeof(struct jzon_value *));
    E->elements[E->capacity - 1] = V;
}

value(V) ::= object(O). {
    V = calloc(1, sizeof(struct jzon));
    V->type = JZON_OBJECT;
    V->object = O;
}
value(V) ::= array(A). {
    V = calloc(1, sizeof(struct jzon));
    V->type = JZON_ARRAY;
    V->array = A;
}
value(V) ::= NUMBER(N). {
    V = calloc(1, sizeof(struct jzon));
    V->type = JZON_NUMBER;
    V->number = atof(N);
    free(N);
}
value(V) ::= STRING(S). {
    V = calloc(1, sizeof(struct jzon));
    V->type = JZON_STRING;
    V->string = string_remove_quotes(S);
    free(S);
}
value(V) ::= TRUE. {
    V = calloc(1, sizeof(struct jzon));
    V->type = JZON_BOOLEAN;
    V->boolean = 1;
}
value(V) ::= FALSE. {
    V = calloc(1, sizeof(struct jzon));
    V->type = JZON_BOOLEAN;
    V->boolean = 0;
}
value(V) ::= NUL. {
    V = calloc(1, sizeof(struct jzon));
    V->type = JZON_NULL;
}
