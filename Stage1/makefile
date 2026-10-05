CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -g
LDLIBS  := -lcunit
SRCS    := main.c cstring.c tokens.c special_tokens.c parser.c executor.c env.c
OBJS    := $(SRCS:.c=.o)

.PHONY: all clean test

all: prog

prog: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule: any .o comes from the matching .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Explicit header dependencies (make can't infer these on its own)
main.o: tokens.h env.h
cstring.o: cstring.h
tokens.o: tokens.h cstring.h const.h
special_tokens.o: tokens.h
parser.o: parser.h
executor.o: executor.h
env.o: env.h cstring.h
special_tokens.o: tokens.h


clean:
	rm -f $(OBJS) cstring_test.o prog test_cstring