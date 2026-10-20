#ifndef ENV_H
#define ENV_H

#define MAX_ENV_VARS 256
#define MAX_PUT_NAME_SIZE 16

/** @brief Null-terminated array of "NAME=value" strings. */
extern const char ** environ;

/** @brief Initializes the environment table. */
void init_env();

/**
 * @brief Looks up an environment variable by name.
 * @param name Variable name
 * @param cache Index hint, updated on a match. Must not be NULL
 * @return The variable's value, or NULL if not found
 * @note If init_env() hasn't been called, always returns NULL
 */
const char * get_env(const char * name, int * cache);

/**
 * @brief Adds or replaces an environment variable.
 * @param buffer String in "NAME=value" form, stored by pointer (not copied)
 * @param cache Index hint, updated when a variable is replaced. Must not be NULL
 * @return 0 on success, -1 on failure with errno set to one of:
 *         - EINVAL: buffer or cache is NULL, buffer has no '=', or the name is empty
 *         - ENAMETOOLONG: the name is longer than MAX_PUT_NAME_SIZE
 *         - ENOMEM: the table already holds MAX_ENV_VARS entries
 * @note If init_env() hasn't been called, returns -1 without setting errno
 */
int put_env(const char *buffer, int *cache);

#endif