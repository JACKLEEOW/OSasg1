#include <unistd.h>
#include <sys/types.h>

#include "cstring.h"
#include "env.h"

#define MAX_PATH 512

static char pwd_buffer[MAX_PATH + 1];
static int pwd_env_cache = -1;

int set_cwd(const char * path) {
    size_t path_size = c_strlen(path);

    if (path_size > MAX_PATH) return -1;
    if (file_exists(path) == -1) return -1;

    c_strcpy(pwd_buffer, path);
    put_env("PWD", pwd_buffer, &pwd_env_cache);

    return 0;
}

const char * get_cwd() {
    return get_env("PWD", &pwd_env_cache);
}

int execvpe(const char *filename, char *const argv[], char *const envp[]) {
    if (filename == NULL) {
        return -1;
    }

    if (file_exists(filename) == 0) {
        execve(argv[0], argv, envp);
        return -1;
    } 

    // TODO:
    // Caching

    // Not defined on function stack
    static char filepath_buffer[MAX_PATH + 1];

    static int path_env_cache = -1;
    const char * paths = get_env("PATH", &path_env_cache);
    
    size_t filename_size = c_strlen(filename);
    // Check Paths (linear trial and error with no cache)
    int pi = 0;
    int pf = 0;
    int fi = 0;

    while (paths[pf] != '\0') {
        pf = pi;
        // copy current environ path
        while (paths[pf] != '\0' && paths[pf] != ':') {
            filepath_buffer[pf] = paths[pf];
            pf++;
        }

        // Mid way error check
        size_t path_size = pf - pi;
        if (path_size + filename_size > MAX_PATH) continue;

        // cat filename
        while (filename[fi] != '\0') {
            filepath_buffer[pf + fi] = filename[fi];
            fi++;
        }
        filepath_buffer[pf + fi] = '\0';

        // Check if this is the correct file path
        if (file_exists(filepath_buffer) == 0) {
            execve(argv[0], argv, envp);
            return -1;
        } 

        pi = pf + 1;
    } 

    return -1;
}

int file_exists(const char * filepath) {
    return (access(filepath, F_OK) == 0) ? 0 : -1
}