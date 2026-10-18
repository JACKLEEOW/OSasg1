#ifndef PATH_H
#define PATH_H

#define MAX_PATH 512

#include <stddef.h>

const char * cat_home(const char * path, char * buffer, size_t size);
/**
 * @brief
 * @param path
 * 
 * @return
 */
int path_exists(const char * path);

/**
 * @brief
 * 
 * @return
 */
const char * get_path();

/**
 * @brief
 * 
 * @return
 */
const char * get_cwd();

/**
 * @brief
 * 
 * @return
 */
const char * get_pwd();

/**
 * @brief
 * 
 * @return
 */
const char * get_oldpwd();

/**
 * @brief
 * 
 * @return
 */
const char * get_home();

/**
 * @brief
 * @param path
 * 
 * @return
 */
int change_cwd(const char * path);

/**
 * @brief
 * @param filename
 * @param argv
 * @param envp
 * 
 * @return
 */
int execvpe(const char *filename, char *const argv[], char *const envp[]);

#endif