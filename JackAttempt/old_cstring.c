#include "cstring.h"
#include "stdio.h"
#include <unistd.h>
#define NULLTERM '\0'
int c_stringcmp(const char *a, const char *b) {
  while (*a != NULLTERM) {
    if (*a != *b) {
      return 0;
    }
    a++;
    b++;
  }
  return *a == *b;
}

void flush_stdin(void) {
  while (1) {
    char temp_buff[128];
    ssize_t n = read(STDIN_FILENO, temp_buff, 128);
    if (n <= 0) {
      return;
    }
    for (ssize_t i = 0; i<n; i++) {
        if (temp_buff[i] == '\n') {
            return;
        }
    }
  }
}
