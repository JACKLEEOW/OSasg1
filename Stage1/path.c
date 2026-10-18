#include <unistd.h>
// #include <sys/types.h>
#include <errno.h>
// #include <stdio.h>

#include "cstring.h"
#include "env.h"
#include "path.h"

static const char * DEFAULT_PATH = "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games";

static int pwd_env_cache = -1;
static int oldpwd_env_cache = -1;
static int home_env_cache = -1;
static int path_env_cache = -1;

// Helper function defs
const char * cat_home(const char * path, char * buffer, size_t size) {
    const char * home = get_home();

    if (home == NULL) { errno = ENOENT; return NULL; } // Home was not found
    if (c_strlen(path) + c_strlen(home) > size) { errno = ENAMETOOLONG; return NULL; } // Path too large

    c_strcpy(buffer, home);
    c_strcat(buffer, path);

    return buffer;
}

int path_exists(const char * path) {
    return (access(path, F_OK) == 0) ? 0 : -1;
}

const char * get_path() {
    const char * path = get_env("PATH", &path_env_cache);

    if (path != NULL) return path;

    put_env(DEFAULT_PATH, &path_env_cache);
    path = get_env("PATH", &path_env_cache);

    if (path != NULL) return path;

    return NULL;
}

const char * get_cwd() {
    static char cwd_buffer[MAX_PATH + 1];
    return getcwd(cwd_buffer, MAX_PATH + 1);
}

const char * get_home() {
    return get_env("HOME", &home_env_cache);
}

const char * get_pwd() {
    return get_env("PWD", &pwd_env_cache);
}

const char * get_oldpwd() {
    return get_env("OLDPWD", &oldpwd_env_cache);
}

int change_cwd(const char * path) {

    static char oldpwd_env[MAX_PATH + 8] = "OLDPWD="; // Must be static
    static char pwd_env[MAX_PATH + 5] = "PWD="; // Must be static

    if (path == NULL) { errno = EINVAL; return -1; } // Path invalid
    if (c_strlen(path) > MAX_PATH) { errno = ENAMETOOLONG; return -1; } // Path too large

    if (chdir(path) < 0) return -1; // Chdir failed

    char pwd_buffer[MAX_PATH + 1] = "";
    char oldpwd_buffer[MAX_PATH + 1] = "";

    const char * oldpwd = get_env("PWD", &pwd_env_cache); // getcwd(oldpwd_buffer, MAX_PATH + 1);
    int is_oldpwd_null = (oldpwd == NULL) ? 1 : 0;
    size_t oldpwd_length = c_strlen(oldpwd);
    
    if (oldpwd_length <= MAX_PATH) c_strcpy(oldpwd_buffer, oldpwd);

    if (getcwd(pwd_buffer, MAX_PATH + 1) == NULL) return -2;  // Failed to copy cwd to buffer (buffer too small) (errno set)

    c_strcpy(pwd_env + 4, pwd_buffer);
    if (put_env(pwd_env, &pwd_env_cache) < 0) return -2; // Path changed, failed to save pwd to the environment (errno set)
    
    if (is_oldpwd_null) { errno = EINVAL; return -3; }
    if (oldpwd_length > MAX_PATH) { errno = ENAMETOOLONG; return -3; }

    c_strcpy(oldpwd_env + 7, oldpwd_buffer);
    if (put_env(oldpwd_env, &oldpwd_env_cache) < 0) return -3; // Path changed, failed to save oldpwd to the environment (errno set)

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

    const char * paths = get_path();
    if (paths == NULL) { errno = ENOENT; return -1; } // PATH environment failed
    
    size_t filename_size = c_strlen(filename);

    // Check Paths (linear trial and error with no cache)
    size_t pi = 0;
    size_t pf = 0;
    size_t fi = 0;
    size_t bf = 0;

    while (paths[pf] != '\0') {
        fi = 0;
        bf = 0;
        pf = pi;

        // Copy current environ path
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

    errno = ENOENT;
    return -1; // Command wasn't found
}