#ifndef MY_HEAP_H
#define MY_HEAP_H
#include <stddef.h>

typedef struct Block {
  size_t size;
  int is_free;  // 1 if free, 0 if allocated
  struct Block *nextp;
} Block;

// use like char *ptr = alloc(wanted_size);
char *alloc(unsigned int size);


void free_all();

// reset the heap
void free(void *ptr);

#endif