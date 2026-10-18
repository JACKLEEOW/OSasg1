#ifndef ENV_H
#define ENV_H

#define MAX_ENV_VARS 256
#define MAX_PUT_NAME_SIZE 16

/**
 * @brief
 */
extern const char ** environ;

/**
 * @brief
 */
void init_env();

/**
 * @brief
 * @param name
 * @param cache
 * @return
 */
const char * get_env(const char * name, int * cache);

/**
 * @brief
 * 
 * @param name
 * @param buffer
 * @param cache
 * @return
 */
int put_env(const char * buffer, int * cache);

#endif