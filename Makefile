PREFIX?=/usr/local
INCLUDE_DIR=$(PREFIX)/include
LIB_DIR=$(PREFIX)/lib

LIB = libjzon.a
HEADER = jzon.h

CFLAGS = -g -Wall -std=gnu11 -fPIC
LDFLAGS =
LDLIBS =
YACC = ./lemon
YFLAGS = -q
LEX = flex
LFLAGS = --header-file=lexer.h

SOURCES = parser.c lexer.c jzon.c object.c array.c
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all release install uninstall test valgrind style clean dist-clean

all: $(LIB)

$(LIB): $(OBJECTS)
	ar -rcs $@ $^

parser.c: lemon parser.y
	$(YACC) $(YFLAGS) parser.y

lexer.c: lexer.l parser.c
	$(LEX) $(LFLAGS) -o $@ $<

release: CFLAGS = -std=gnu11 -Os -march=native -flto -Wall -Wextra -Wpedantic -Wstrict-overflow -fno-strict-aliasing
release: $(LIB)

install: release
	mkdir -p $(INCLUDE_DIR)
	mkdir -p $(LIB_DIR)
	install -m 644 $(LIB) $(LIB_DIR)/$(LIB)
	install -m 644 $(HEADER) $(INCLUDE_DIR)/$(HEADER)

uninstall:
	rm -f $(LIB_DIR)/$(LIB)
	rm -f $(INCLUDE_DIR)/$(HEADER)

SPECK_CFLAGS = -I.
SPECK_LDFLAGS = -L.
SPECK_LIBS = -ljzon
-include speck/speck.mk
test: $(SPECK) $(LIB) $(SUITES)
	@$(SPECK)

lemon: lemon.c
	$(CC) -std=gnu11 -Os -Wall -o $@ $<

valgrind: $(SPECK) $(LIB) $(SUITES)
	@valgrind --leak-check=full --error-exitcode=1 $(SPECK)

style:
	astyle -A3s4SpHk3jn jzon.c jzon.h object.c array.c spec/*.c

clean:
	rm -f $(LIB) $(OBJECTS) parser.c parser.h lexer.c lexer.h
	rm -rf *.dSYM spec/*.dSYM
	rm -f spec/*.so

dist-clean:
	@make clean
	rm -f lemon
