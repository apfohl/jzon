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

    struct jzon *array = jzon_parse("[1, 2, 3]");
    struct jzon *object = jzon_parse("{\"number\": 42}");

If an error is encountered during the parsing, `jzon_parse` will return `NULL`.

To release the obtained memory by `jzon_parse`, you must call `jzon_free` on the
object when you are done:

    jzon_free(jzon);

### Accessing the JSON data

The API provides functionality to determine what was parsed:

- `jzon_is_object(struct jzon *jzon)`: Returns `1` if `jzon` is an object
- `jzon_is_array(struct jzon *jzon)`: Returns `1` if `jzon` is an array
- `jzon_is_number(struct jzon *jzon)`: Returns `1` if `jzon` is a number
- `jzon_is_string(struct jzon *jzon)`: Returns `1` if `jzon` is a string
- `jzon_is_boolean(struct jzon *jzon)`: Returns `1` if `jzon` is a boolean
- `jzon_is_null(struct jzon *jzon)`: Returns `1` if `jzon` is null

If `jzon_is_object` returns `1`, you can use `jzon_object_get` to access the
members of that object:

    struct jzon *member = jzon_object_get(jzon, "key");

If `jzon_is_array` returns `1`, you can use the functions `jzon_array_size` and
`jzon_array_get`:

    for (int i = 0; i < jzon_array_size(jzon); i++) {
        struct jzon *element = jzon_array_get(jzon, i);
    }

if `jzon_is_number`, `jzon_is_string` or `jzon_is_boolean` returns `1`, you can
access the objects data:

    if (jzon_is_number(jzon)) {
        double number = jzon->number;
    }

    if (jzon_is_string(jzon)) {
        char *string = jzon->string;
    }

    if (jzon_is_boolean(jzon)) {
        unsigned int boolean = jzon->boolean;
    }

### Error handling

JZON provides a global error variable `jzon_error`. It contains the following
members:

- `error`: an enum that defines the error type
- `msg`: a null terminated string that explains the error

The error enum can have the following values:

- `JZONE_NONE`
- `JZONE_OUT_OF_MEMORY`
- `JZONE_INVALID_INPUT`
- `JZONE_LEXER_ERROR`
- `JZONE_PARSER_ERROR`

Error handling example:

    if (jzon_error.error != JZONE_NONE) {
        fprintf(stderr, "%s\n", jzon_error.msg);
    }

Happy JSONing.
