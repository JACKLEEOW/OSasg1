#include <unistd.h>
#include <fcntl.h>

#include "const.h"
#include "cstring.h"
#include "io.h"
#include "tokens.h"
#include "parser.h"
#include "executor.h"
#include "env.h"
#include "path.h"

void get_user_input(char * input_buffer);
void run_user_input(const char * input_buffer);
void flush_input_buffer();

int main() {
    init_env();
    char input_buffer[MAX_INPUT_SIZE + 1] = {'\0'};

    do {
        get_user_input(input_buffer);
        run_user_input(input_buffer);
    } while (1);

    return 0;
}

void get_user_input(char * input_buffer) {
    do {
        const char * wd = get_cwd();
        WRITE_OUT(MYSH_START, DARK_ORANGE, wd, GREEN "$ " RESET);

        ssize_t bytes_read = read(STDIN_FILENO, input_buffer, MAX_INPUT_SIZE + 1);

        if (bytes_read > MAX_INPUT_SIZE) {
            WRITE_ERR(ERROR_START " Exceeded max character limit ("TO_STRING(MAX_INPUT_SIZE)")\n");
            flush_input_buffer();
            continue;
        }

        input_buffer[bytes_read] = '\0';
        size_t str_size = c_trim(input_buffer);

        if (str_size > 0) break;

    } while (1);
}

void run_user_input(const char * input_buffer) {
    Tokens t;
    Pipeline p;

    create_tokens(&t, input_buffer);
    
    //test_tokens(&t);

    int parse_status = parse_tokens(&t, &p);
    if (parse_status != PAR_OK) return;
    //test_parse(&p, parse_status);
    execute_pipeline(&p);

}

void flush_input_buffer() {

    /*
        Issue was that whenever I tried to flush something the exact same size as my max 
        input plus one, it would require the user to type something for the flush to go through
        Here, I am unblocking the fd0 stream so that I don't have to rely on the user.
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
