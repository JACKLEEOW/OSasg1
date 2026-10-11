#ifndef PATH_H
#define PATH_H

#define MAX_PATH 512

/**
 * @brief
 * @param path
 * 
 * @return
 */
int path_exists(const char * path);

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

/**
 * @brief
 * 
 * @return
 */
const char * get_pwd();

#endif