#ifndef ENV_H
#define ENV_H

#define MAX_ENV_VARS 256

/**
 * @brief
 */
extern const char ** environ; // envp

/**
 * @brief
 */
void steal_and_impersonate_env(); // basically init for env

/**
 * @brief
 * @param name
 * @param cache
 * @return
 */
const char * get_env(const char * name, int * cache); // get_

/**
 * @brief
 * 
 * @param name
 * @param buffer
 * @param cache
 * @return
 */
int put_env(const char * name, const char * buffer, int * cache);

#endif