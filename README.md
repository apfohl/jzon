# JZON

[![Build Status](https://travis-ci.org/apfohl/jzon.svg?branch=master)](https://travis-ci.org/apfohl/jzon)

JZON is a small and easy to use JSON parser for the C programming language. It
is designed to be developer friendly. JZON is compliant to the
[ECMA-404](http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf)
standard.

## Installation

### OS X with Homebrew

If you use Homebrew under OS X you can use the tap `apfohl/brew`:

    $ brew tap apfohl/brew
    $ brew install jzon

### Manually

JZON comes as a static linkable library. In order to use it, you need to compile
it first.

To build a release version you need to run the following command:

    $ make release

To make the library available for use, you have to install it to the right
location. You can set the `PREFIX` environment variable to set the installation
path. The default is `/usr/local`. The installation is done by the following
command:

    $ make install

or, if you want to change the `PREFIX`:

    $ make install PREFIX=/your/prefix

Of cause you can uninstall it with the following command:

    $ make uninstall

## Usage

To use JZON in your projects, you need to link against the library. If you have
installed the library to the default `PREFIX` or with Homebrew, you can simply
use the `-ljzon` flag for compilation.

Example:

    $ clang -o app app.c -ljzon

## API

To get the API available you need to include the `jzon.h` header file:

    #include <jzon.h>

The main JSON object type is `struct jzon`. It can ether be a JSON object or a
JSON array.

### Parsing a JSON string

    enum jzon_error_type error;
    struct jzon *array = jzon_parse("[1, 2, 3]", &error);
    struct jzon *object = jzon_parse("{\"number\": 42}", &error);

If an error is encountered during the parsing, `jzon_parse` will return `NULL`
and the `error` variable is set to the appropiate error value.

To release the obtained memory by `jzon_parse`, you must call `jzon_free` on the
object when you are done:

    jzon_free(jzon);

### Accessing the JSON data

The API provides functionality to determine what was parsed:

- `jzon_is_object(struct jzon *jzon, enum jzon_error_type *error)`: Returns `1` if `jzon` is an object
- `jzon_is_array(struct jzon *jzon, enum jzon_error_type *error)`: Returns `1` if `jzon` is an array
- `jzon_is_number(struct jzon *jzon, enum jzon_error_type *error)`: Returns `1` if `jzon` is a number
- `jzon_is_string(struct jzon *jzon, enum jzon_error_type *error)`: Returns `1` if `jzon` is a string
- `jzon_is_boolean(struct jzon *jzon, enum jzon_error_type *error)`: Returns `1` if `jzon` is a boolean
- `jzon_is_null(struct jzon *jzon, enum jzon_error_type *error)`: Returns `1` if `jzon` is null

Create an `error` variable:

    enum jzon_error_type error;

If `jzon_is_object` returns `1`, you can use `jzon_object_get` to access the
members of that object:

    struct jzon *member = jzon_object_get(jzon, "key", &error);

If `jzon_is_array` returns `1`, you can use the functions `jzon_array_size` and
`jzon_array_get`:

    for (int i = 0; i < jzon_array_size(jzon, &error); i++) {
        struct jzon *element = jzon_array_get(jzon, i, &error);
    }

if `jzon_is_number`, `jzon_is_string` or `jzon_is_boolean` returns `1`, you can
access the objects data:

    if (jzon_is_number(jzon, &error)) {
        double number = jzon->number;
    }

    if (jzon_is_string(jzon, &error)) {
        char *string = jzon->string;
    }

    if (jzon_is_boolean(jzon, &error)) {
        unsigned int boolean = jzon->boolean;
    }

### Error handling

Error handling with JZON is done by assigning an error variable to every API
function. The error variable is of type `enum jzon_error_type` and can have the
following values:

- `JZONE_NONE`
- `JZONE_OUT_OF_MEMORY`
- `JZONE_INVALID_INPUT`
- `JZONE_LEXER_ERROR`
- `JZONE_PARSER_ERROR`
- `JZONE_NO_ENTRY`,
- `JZONE_ARRAY_OUT_OF_BOUNDS`

Error handling example:

    enum jzon_error_type error;
    struct jzon *member = jzon_object_get(jzon, "key", &error);

    if (error != JZONE_NONE) {
        fprintf(stderr, "%s\n", get_error_string(error));
    }

By using the API function `get_error_string` you can turn the error value into a
readable string.

If you want to turn off error handling, you can pass `NULL` instead of an error
variable. If so, no error handling is done by JZON.

Happy JSONing.
