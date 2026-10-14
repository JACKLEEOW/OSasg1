#include <unistd.h>
#include <stddef.h>

#include "const.h"
#include "cstring.h"

void write_error(const char *message) {
    if (message == NULL) return;
    write(STDERR_FILENO, message, c_strlen(message));
}

void write_out(const char *message) {
    if (message == NULL) return;
    write(STDOUT_FILENO, message, c_strlen(message));
}