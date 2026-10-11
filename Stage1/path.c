#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include "cstring.h"
#include "env.h"
#include "path.h"

static char pwd_buffer[MAX_PATH + 5] = "PWD=";
static int pwd_env_cache = -1;

static const char * DEFAULT_PATH = "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games";

int path_exists(const char * path) {
    return (access(path, F_OK) == 0) ? 0 : -1;
}

int change_cwd(const char * path) {
    if (path == NULL) return -1; // Path invalid

    char cwd_buffer[MAX_PATH + 1];

    if (chdir(path) == -1) return -1; // Chdir failed
    if (getcwd(cwd_buffer, MAX_PATH + 1) == NULL) return -1;  // Failed to copy cwd to buffer (buffer too small)

    c_strcpy(pwd_buffer + 4, cwd_buffer);
    if (put_env("PWD", pwd_buffer, &pwd_env_cache)) return -1; // Failed to save to the environment

    return 0; // Success
}

int execvpe(const char *filename, char *const argv[], char *const envp[]) {
    if (filename == NULL || filename[0] == '\0') return -1; // Invalid name

    if (path_exists(filename) == 0) {
        execve(argv[0], argv, envp);
        return -1;  // Execve failed
    } 

    // TODO:
    // Caching

    static char filepath_buffer[MAX_PATH + 1];
    static int path_env_cache = -1;

    const char * paths = get_env("PATH", &path_env_cache);
    if (paths == NULL) {
        put_env("PATH", DEFAULT_PATH, &path_env_cache);
        paths = get_env("PATH", &path_env_cache);
        if (paths == NULL) return -1; // setting PATH environment failed
    }
    
    size_t filename_size = c_strlen(filename);

    // Check Paths (linear trial and error with no cache)
    int pi = 0;
    int pf = 0;
    int fi = 0;
    int bf = 0;

    while (paths[pf] != '\0') {
        fi = 0;
        bf = 0;
        pf = pi;

        // copy current environ path
        while (paths[pf] != '\0' && paths[pf] != ':') {
            filepath_buffer[bf++] = paths[pf++];
        }

        filepath_buffer[bf++] = '/';
        // Mid way error check
        size_t path_size = pf - pi;
        if (path_size + filename_size + 1 > MAX_PATH) {
            continue;
        }

        // cat filename
        while (filename[fi] != '\0') {
            filepath_buffer[bf + fi] = filename[fi];
            fi++;
        }
        filepath_buffer[bf + fi] = '\0';

        // Check if this is the correct file path
        if (path_exists(filepath_buffer) == 0) {
            execve(filepath_buffer, argv, envp);  
            return -1; // Execve failed
        } 

        pi = pf + 1;
    } 

    return -1; // Command wasn't found
}

const char * get_pwd() {
    return get_env("PWD", &pwd_env_cache);
}