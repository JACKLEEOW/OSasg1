#include "cstring.h"
#define NULLTERM '\0'
int c_stringcmp(const char* a, const char* b) {
    while( *a != NULLTERM) {
        if (*a != *b) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;


}