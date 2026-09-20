#include "cstring.h"
#include "stdio.h"
#include <unistd.h>
#define BUFFER_SIZE 128
#define DOLLAR_SIGN "$ "
#define EXIT "exit"
#define NEWLINE '\n'
#define ERROR_MSG "ERROR BUFFER LIM REACHED"
#define EXIT_MSG "Powering off ---- Farewell, Master!"
int main() {

  while (1) {
    write(STDOUT_FILENO, DOLLAR_SIGN, 2);
    char buffer[BUFFER_SIZE];

    ssize_t n = read(STDIN_FILENO, buffer, BUFFER_SIZE);
    if (n == BUFFER_SIZE &&
        buffer[n - 1] != '\n') { // if buffer limit is exceeded
      write(STDOUT_FILENO, ERROR_MSG, sizeof(ERROR_MSG) - 1); // write error and
      write(STDOUT_FILENO, "\n",1);      // probably should change these hardcoded vals
      flush_stdin(); // clean buffer
      continue;
    }
    if (n <= 0) {
      break;
    }

    if (n < BUFFER_SIZE) {
      buffer[n] = '\0'; // null terminate buffer
    }
    if (buffer[n - 1] == '\n') { // if user presses enter, change to null term
      buffer[n - 1] = '\0';
    }

    if (buffer[0] == '\0') {
      continue; // empty line case user types enter
    }

    if (c_stringcmp(buffer, EXIT) == 1) {
      write(STDOUT_FILENO, EXIT_MSG, sizeof(EXIT_MSG) - 1);
      return 0;
    }
    write(STDOUT_FILENO, buffer, n);
    write(STDOUT_FILENO, "\n", 1);
  }
  return 0;
}