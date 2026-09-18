#include "cstring.h"
#include <unistd.h>
#include "stdio.h"
#define BUFFER_SIZE 128
#define DOLLAR_SIGN "$"
int main() {

  while (1) {
    char buffer[BUFFER_SIZE];

    char str[] = "hello";
    char str2[] = "hello";

    ssize_t n = read(STDIN_FILENO, buffer, BUFFER_SIZE);
    if (n > 0) {
      write(STDOUT_FILENO, buffer, n);
    }

    if(c_stringcmp(str,str2) == 1) {
        printf("MATCH\n");

    }
    else {
        printf("NO MATCH\n");
    }

 
  }

  return 0;
}
