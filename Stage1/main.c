#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "const.h"
#include "cstring.h"
#include "io.h"
#include "tokens.h"
#include "parser.h"
#include "executor.h"
#include "env.h"
#include "path.h"
#include "signals.h"
#include "job_control.h"

/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

static void get_user_input(char * input_buffer);
static void run_user_input(const char * input_buffer);
static void flush_input_buffer();

/* ---------------------------------- MAIN ---------------------------------- */

int main() {
    init_env();
    init_shell_signals();
    char input_buffer[MAX_INPUT_SIZE + 2] = {'\0'};

    do {
        get_user_input(input_buffer);
        run_user_input(input_buffer);
    } while (1);

    return 0;
}

/* ----------------------- HELPER FUNCTION DEFINITIONS ---------------------- */

static void get_user_input(char * input_buffer) {
    do {

        // Zombies are collected right before each prompt
        reap_jobs();

        const char * wd = get_cwd();
        WRITE_OUT(MYSH_START, DARK_ORANGE, wd, GREEN "$ " RESET);

        ssize_t bytes_read = read(STDIN_FILENO, input_buffer, MAX_INPUT_SIZE + 2);

        // Error while reading
        if (bytes_read < 0) {
            // Interupted by signal, ignore
            if (errno == EINTR) {
                continue;
            }

            // Critical by signal, ignore
            WRITE_ERR(ERROR_START " Failed read\n");
            _exit(1);
        
        // Control-D on empty line
        } else if (bytes_read == 0) {
            WRITE_OUT("\n");
            continue;

        // Input too big
        } else if (bytes_read > MAX_INPUT_SIZE + 1) {
            WRITE_ERR(ERROR_START " Exceeded max character limit ("TO_STRING(MAX_INPUT_SIZE)")\n");
            flush_input_buffer();
            continue;
        }

        // Add null terminator
        input_buffer[bytes_read] = '\0';
        size_t str_size = c_trim(input_buffer);

        if (str_size > 0) break;

    } while (1);
}

static void run_user_input(const char * input_buffer) {
    Tokens t;
    Pipeline p;

    create_tokens(&t, input_buffer);
    int parse_status = parse_tokens(&t, &p);
    if (parse_status != PAR_OK) return;
    execute_pipeline(&p);

}

static void flush_input_buffer() {

    // Found online,

    // Get current file descriptor flags
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags == -1) return;

    // Set to non-blocking mode
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    char buffer[MAX_INPUT_SIZE];
    while (read(STDIN_FILENO, buffer, MAX_INPUT_SIZE) > 0);

    // Restore original blocking behavior
    fcntl(STDIN_FILENO, F_SETFL, flags);
}
