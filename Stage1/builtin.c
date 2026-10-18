#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>

#include "builtin.h"
#include "io.h"
#include "const.h"
#include "path.h"

int exit_builtin(const char * argv[], int argc) {
    if (argc > 2) {
        WRITE_ERR(ERROR_START " exit: Too many arguments \n");
        return -1;
    }

    if (argc == 1) {
        _exit(0);
    } 

    WRITE_ERR(MYSH_START " exited with status '", argv[1], "'\n");
    _exit(1);
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