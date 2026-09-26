#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "const.h"
#include "cstring.h"
#include "tokens.h"

const char CHAR_LIMIT_EXCEEDED[] =  RED "exceeded max character limit ("TO_STRING(MAX_INPUT_SIZE)")" RESET;
const char DOLLAR_SIGN[] = GREEN "$ " RESET;

// Tokens t = {
//     .token_strings = {'\0'},
//     .token_string_ptrs = {NULL},
//     .num_tokens = 0
// };
    
void flushFD0();

int main() {
    while (1) {
        char input_buffer[MAX_INPUT_SIZE + 1] = {'\0'};
        ssize_t bytes;

        write(STDOUT_FILENO, DOLLAR_SIGN, c_strlen(DOLLAR_SIGN));
        bytes = read(STDIN_FILENO, input_buffer, MAX_INPUT_SIZE + 1);

        // if read over max input limit
        if (bytes > MAX_INPUT_SIZE) {
            write(STDOUT_FILENO, CHAR_LIMIT_EXCEEDED, c_strlen(CHAR_LIMIT_EXCEEDED));
            write(STDOUT_FILENO, "\n", 1);
            flushFD0();
            continue;
        }

        size_t str_size = c_trim(input_buffer);
        // size_t strSize = bytes;

        if (c_strcmp(input_buffer, "exit") == 0) {
            return 0;

        } else if (str_size > 0) {
            Tokens t;
            create_tokens(&t, input_buffer);
            write(STDOUT_FILENO, input_buffer, str_size);
            write(STDOUT_FILENO, "\n", 1);

            test_tokens(&t);
        }

    }

    return 0;
}

void flushFD0() {

    /**
        Issue was that whenever I tried to flush something the exact same size as my max input plus one, it would require the user to type something for the flush to go through
        Here, I am unblocking the fd0 stream so that I don't have to relie on the user.
    */

    // found online

    // get current file descriptor flags
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags == -1) return;

    // set to non-blocking mode
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    char buffer[MAX_INPUT_SIZE];
    while (read(STDIN_FILENO, buffer, MAX_INPUT_SIZE) > 0) {}

    // restore original blocking behavior
    fcntl(STDIN_FILENO, F_SETFL, flags);
}
