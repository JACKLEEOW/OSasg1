
all: main.o cstring.o tokens.o
	gcc main.o cstring.o tokens.o -o prog

cstring_test: cstring_test.o cstring.o cstring.h
	gcc cstring_test.o cstring.o -o test_cstring -lcunit

main.o: main.c tokens.h
	gcc -c main.c

cstring_test.o: test_cstring.c cstring.h
	gcc -c test_cstring.c

cstring.o: cstring.c cstring.h 
	gcc -c cstring.c

tokens.o: tokens.c tokens.h cstring.h const.h
	gcc -c tokens.c

clean:
	rm *.o prog test_cstring
