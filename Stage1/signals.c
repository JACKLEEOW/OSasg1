#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "signals.h"
#include "io.h"
#include "const.h"

/* -------------------------------- HANDLERS -------------------------------- */

static void handle_sigchld(int sig) { 
    (void)sig;

    int saved_errno = errno;

    // while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

static void handle_sigint(int sig) { 
    (void)sig;

    int saved_errno = errno;

    write(STDOUT_FILENO, "\n", 1);

    errno = saved_errno;
}

/* --------------------------- FUNCTION DEFINITONS -------------------------- */

int init_shell_signals() {
    struct sigaction sa = {0};
    // memset(&sa, 0, sizeof(sa));
    int success_flag = 0;

    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        success_flag = -1;
    }

    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        success_flag = -1;
    }
    
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;

    if (sigaction(SIGTSTP, &sa, NULL) < 0) {
        success_flag = -1;
    }

    if (sigaction(SIGTTOU, &sa, NULL) < 0) {
        success_flag = -1;
    }
    
    return success_flag;
}

int restore_children_default_signals() {
    struct sigaction sa = {0};
    //memset(&sa, 0, sizeof(sa));
    int success_flag = 0;

    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        success_flag = -1;
    }
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        success_flag = -1;
    }
    if (sigaction(SIGTSTP, &sa, NULL) < 0) {
        success_flag = -1;
    }
    if (sigaction(SIGTTOU, &sa, NULL) < 0) {
        success_flag = -1;
    }

    return success_flag;
}

