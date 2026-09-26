CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -g
LDLIBS  := -lcunit
SRCS    := main.c cstring.c tokens.c special_tokens.c
OBJS    := $(SRCS:.c=.o)

.PHONY: all clean test

all: prog

prog: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule: any .o comes from the matching .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Explicit header dependencies (make can't infer these on its own)
main.o: tokens.h
cstring.o: cstring.h
tokens.o: tokens.h cstring.h const.h
special_tokens.o: tokens.h
cstring_test.o: cstring.h

test: test_cstring
	./test_cstring

test_cstring: cstring_test.o cstring.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(OBJS) cstring_test.o prog test_cstring