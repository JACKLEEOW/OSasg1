#include <unistd.h>
#include "cstring.h"
#include "const.h"
#include "tokens.h"
#include <stdio.h>
#include <fcntl.h>

#define RED   "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

const char CHAR_LIMIT_EXCEEDED[] =  RED "exceeded max character limit ("TO_STRING(MAX_INPUT_SIZE)")" RESET;
const char DOLLAR_SIGN[] = GREEN "$ " RESET;

void flushFD0();

int main(int argc, char ** argv) {
    while (1) {
        char inputBuffer[MAX_INPUT_SIZE + 1] = {'\0'};
        ssize_t bytes;

        write(STDOUT_FILENO, DOLLAR_SIGN, c_strlen(DOLLAR_SIGN));
        // if (bytes == ERROR_NUMBER) {
        //     return -1;
        // }

        bytes = read(STDIN_FILENO, inputBuffer, MAX_INPUT_SIZE + 1);

        // if read over max input limit
        if (bytes > MAX_INPUT_SIZE) {
            write(STDOUT_FILENO, CHAR_LIMIT_EXCEEDED, c_strlen(CHAR_LIMIT_EXCEEDED));
            write(STDOUT_FILENO, "\n", 1);
            flushFD0();
            continue;
        }
        
        // printf("INPUT og: _>");
        // print_hidden_string(inputBuffer);
        //size_t strSize = c_trim(inputBuffer);
        size_t strSize = bytes;
        if (c_strcmp(inputBuffer, "exit") == 0) {
            return 0;
        }
        else if (strSize > 0) {
            Tokens t = createTokens(inputBuffer);
            write(STDOUT_FILENO, inputBuffer, strSize);
            write(STDOUT_FILENO, "\n", 1);

            // testTokens(&t);
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
