#include "myheap.h"


#define HEAP_SIZE 10000     /* adjust as necessary */

static char heap[HEAP_SIZE];
static char *freep = heap;


char *alloc(unsigned int size)
{
    if(freep + size >= heap + HEAP_SIZE){
        return 0; // Allocation overflow doesn't fit into heap
    }
    char *start_allocation = freep;
    freep = freep + size;

    return start_allocation;
};


void free_all()
{
    freep = heap;
}