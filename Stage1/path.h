#ifndef PATH_H
#define PATH_H

#include <stddef.h>

/* --------------------------------- MACROS --------------------------------- */

#define MAX_PATH 512

/* --------------------------- FUNCTION PROTOTYPES -------------------------- */

const char * cat_home(const char * path, char * buffer, size_t size);

int path_exists(const char * path);

const char * get_path();

const char * get_cwd();

const char * get_pwd();

const char * get_oldpwd();

const char * get_home();

int change_cwd(const char * path);

int execvpe(const char *filename, char *const argv[], char *const envp[]);

#endif