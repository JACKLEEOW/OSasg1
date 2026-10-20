#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <signal.h>

#include "builtin.h"
#include "io.h"
#include "const.h"
#include "path.h"
#include "job_control.h"
#include "cstring.h"

// int fg_builtin(const char * argv[], int argc) {
//     if (argc > 2) {
//         WRITE_ERR(ERROR_START " exit: Too many arguments \n");
//         return -1;
//     }

//     if (argc == 1) {
//         Job *job = focused_job();
//         tcsetpgrp(STDIN_FILENO, job->pgid);

        
//         _exit(0);
//     } 

//     WRITE_ERR(MYSH_START " exited with status '", argv[1], "'\n");
//     _exit(1);
//     return -1;
// }

/* --------------------------- FUNCTION DEFINITONS -------------------------- */

int kill_builtin(const char * argv[], int argc) {
    if (argc > 2) {
        WRITE_ERR(ERROR_START " kill: Too many arguments \n");
        return -1;
    }

    Job *job;
    if (argc == 1) {
        job = focused_job();
    } else {
        unsigned int id = cstr_to_uint(argv[1]);
        char buffer[3];
        const char *id_text = uint_to_cstr(buffer, 3, id);
        WRITE_OUT(MYSH_START " kill: Using id=", id_text, "\n");
        job = get_job((int)id);
    }

    if (job == NULL) {
        WRITE_ERR(ERROR_START " kill: Job not found \n");
        return -1;
    }

        kill(-job->pgid, SIGKILL);
        int status;
        pid_t w;
    // Wait for job to die
    while ((w = waitpid(-job->pgid, &status, 0)) > 0) update_job_status(job, status);
    return 0;
}

int jobs_builtin(const char * argv[], int argc) {
    (void)argv;
    if (argc > 1) {
        WRITE_ERR(ERROR_START " jobs: Too many arguments \n");
        return -1;
    }

    display_jobs();

    return 0;
}

int exit_builtin(const char * argv[], int argc) {
    if (argc > 2) {
        WRITE_ERR(ERROR_START " exit: Too many arguments \n");
        return -1;
    }

    if (argc == 1) {
        _exit(0);
    } 

    unsigned int status = cstr_to_uint(argv[1]);
    char buffer[3];
    const char *id_text = uint_to_cstr(buffer, 3, status);
    WRITE_ERR(MYSH_START " exited with status '", id_text, "'\n");
    _exit(status);
    return -1;
}

int cd_builtin(const char * argv[], int argc) {
    if (argc > 2) {
        WRITE_ERR(ERROR_START " cd: Too many arguments \n");
        return -1;
    }

    const char * path = argc == 2 ? path = argv[1] : "";

    char home_path_buffer[MAX_PATH + 1];

    // Home
    if (path[0] == '\0' || (path[0] == '~' && path[1] == '/') || (path[0] == '~' && path[1] == '\0')) {
        if (path[0] == '~') path++; // Skip ~
        
        path = cat_home(path, home_path_buffer, MAX_PATH + 1);
        if (path == NULL) {
            WRITE_ERR(ERROR_START " cd: HOME not set\n");
            return -1;
        }

    // Old PWD
    } else if (path[0] == '-' && path[1] == '\0') {
        path = get_oldpwd();
        if (path == NULL) {
            WRITE_ERR(ERROR_START " cd: OLDPWD not set\n");
            return -1;
        }
    }

    int status = change_cwd(path);
    if (status == -1) {
        WRITE_ERR(ERROR_START " cd: ", strerror(errno), " '", path, "'\n");
        return -1;
    } else if (status == -2) {
        WRITE_ERR(WARNING_START " cd: Directory changed, error while updating PWD\n");
        return -2;
    } else if (status == -3) {
        if (errno == EINVAL) WRITE_ERR(WARNING_START " cd: Directory changed, error while setting OLDPWD\n");
        else WRITE_ERR(WARNING_START " cd: Directory changed, error while updating OLDPWD\n");
        return -3;
    }

    return 0;
}