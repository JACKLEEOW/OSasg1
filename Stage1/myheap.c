#include "myheap.h"
#include <stddef.h>

#define HEAP_SIZE 10000     /* adjust as necessary */

static Block heap[HEAP_SIZE];
static Block *freep = heap;

// use like char *ptr = alloc(wanted_size);
char *alloc(unsigned int size)
{
  if(!size){
    return NULL;        // allocate size 0, or NULL, aka nothing
  }
  if(freep == NULL){
    // initialize the heap.
    freep->size = HEAP_SIZE - sizeof(Block);
    freep->is_free = 1;
    freep->nextp = NULL;
  }

  Block *cur = freep;

  // Go through the current heap
  while(cur){
    // Until it finds a free block big enough for the size of the new allocation
    if(cur->is_free && cur->size >= size){
    // If the size of the current block is bigger than the size needed, split it.
      if (cur->size > size + sizeof(Block)) {

        // Make new free block - the size needed for the new allocation.
        Block *b = (Block*)((char*)cur + sizeof(Block) + size);
        b->size = cur->size - size - sizeof(Block);
        b->is_free = 1;
        b->nextp = cur->nextp;
        cur->nextp = b;
        cur->size = size;
      }

    cur->is_free = 0;
    return (char*)cur + sizeof(Block);
    }
    cur = cur->nextp;
  }
  return NULL; // No more free blocks

}


void free(void *ptr){
  Block *b = (Block *)((char *)ptr - sizeof(Block));

  if(b == NULL){
    return;     // Free nothing
  }

  b->is_free = 1;
  Block *cur = freep;
  // walk the list until end
  while (cur != NULL && cur->nextp != NULL) {

    if (cur->is_free && cur->nextp->is_free) {
      cur->size += sizeof(Block) + cur->nextp->size;
      cur->nextp = cur->nextp->nextp;

      /* check again in case there are more adjacent blocks to merge together */
      continue;
    }

    cur = cur->nextp;
    }

}
void free_all(){
  freep = (Block *)heap;

  freep->size = HEAP_SIZE - sizeof(Block);
  freep->is_free = 1;
  freep->nextp = NULL;
}
