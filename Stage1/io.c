#include <unistd.h>
#include <stddef.h>

#include "io.h"
#include "const.h"
#include "errno.h"
#include "cstring.h"

/* --------------------------------- MACROS --------------------------------- */

#define INTERNAL_BUFFER_SIZE 256

/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

/** @brief Flushes buffer while writing */
static int flush_buffer(int fd, const char *buffer, size_t length) {
    while (length > 0) {
        ssize_t bytes = write(fd, buffer, length);
        if (bytes < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        buffer += bytes;
        length -= (size_t)bytes;
    }
    return 0;
}

/** @brief Write + buffer */
static int write_fd(int fd, const char *const messages[], size_t count) {
    int saved_errno = errno;

    char buffer[INTERNAL_BUFFER_SIZE];
    size_t used = 0;
    
    int return_value = 0;

    if (messages == NULL) return 0;

    for (size_t i = 0; i < count && return_value == 0; i++) {
        const char *message = messages[i];
        if (message == NULL) continue;

        while (*message != '\0') {
            buffer[used++] = *message++;
            if (used == INTERNAL_BUFFER_SIZE) {
                if (flush_buffer(fd, buffer, used) < 0) { return_value = -1; break; }
                used = 0;
            }
        }
    }

    if (return_value == 0 && used > 0 && flush_buffer(fd, buffer, used) < 0) return_value = -1;

    errno = saved_errno;
    return return_value;
}

/* -------------------------- FUNCTION DEFINITIONS -------------------------- */

int write_error(const char *const messages[], size_t count) {
    return write_fd(STDERR_FILENO, messages, count);
}

int write_out(const char *const messages[], size_t count) {
    return write_fd(STDOUT_FILENO, messages, count);
}

#undef INTERNAL_BUFFER_SIZE