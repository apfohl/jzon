LIB = libjzon.a

CFLAGS = -g -Wall -std=gnu11
LDFLAGS =
LDLIBS =
YACC = ./lemon
YFLAGS = -q
LEX = flex
LFLAGS = --header-file=lexer.h

SOURCES = parser.c lexer.c jzon.c object.c array.c
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

.PHONY: all test style clean dist-clean

all: $(LIB)

$(LIB): $(OBJECTS)
	ar -rcs $@ $^

parser.c: lemon parser.y
	$(YACC) $(YFLAGS) parser.y

lexer.c: lexer.l parser.c
	$(LEX) $(LFLAGS) -o $@ $<

lemon: lemon.c
	$(CC) -std=gnu11 -Os -Wall -o $@ $<

SPECK_CFLAGS = -I.
SPECK_LDFLAGS = -L.
SPECK_LIBS = -ljzon
-include speck/speck.mk
test: $(SPECK) $(LIB) $(SUITES)
	@$(SPECK)

style:
	astyle -A3s4SpHk3jn jzon.c jzon.h object.c array.c spec/*.c

clean:
	rm -f $(LIB) $(OBJECTS) parser.c parser.h lexer.c lexer.h
	rm -rf *.dSYM spec/*.dSYM
	rm -f spec/*.so

dist-clean:
	@make clean
	rm -f lemon
