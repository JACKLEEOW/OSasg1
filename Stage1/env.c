#include <stddef.h>
#include <errno.h>

#include "env.h"
#include "cstring.h"

/* --------------------------------- GLOBALS -------------------------------- */

static const char * env_table[MAX_ENV_VARS + 1];
static int env_count = 0;
static int init = 0;

/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

static const char * env_helper(const char * name, int * return_cache, size_t i) {

    const char * env_var = environ[i];
    size_t j = 0;
    while (name[j] == env_var[j]) j++;
    if (name[j] == '\0' && env_var[j] == '=') {
        *return_cache = i;
        return (env_var + j + 1);
    }

    *return_cache = -1;
    return NULL;
}

/* --------------------------- FUNCTION DEFINITONS -------------------------- */

void init_env() {
    int i;
    
    for (i = 0; environ[i] != NULL || i > MAX_ENV_VARS; i++) {
        env_table[i] = environ[i];
        env_count++;
    }

    env_table[i] = NULL;
    environ = env_table;

    init = 1;
}

const char * get_env(const char * name, int * cache) {
    if (init == 0) return NULL;
    if (name == NULL || cache == NULL) return NULL;
    const char * result;

    // Try cache O(1)
    if (*cache >= 0 && *cache < env_count) {
        result = env_helper(name, cache, *cache);
        if (result != NULL) return result;
    } 

    // Try all O(N)
    for (size_t i = 0; environ[i] != NULL; i++) {
        result = env_helper(name, cache, i);
        if (result != NULL) return result;
    }

    *cache = -1;
    return NULL;
}

int put_env(const char * buffer, int * cache) {
    if (init == 0) return -1;
    if (buffer == NULL || cache == NULL) { errno = EINVAL; return -1; }

    // Get Name
    char name[MAX_PUT_NAME_SIZE + 1]; // Should really be dynamic
    size_t name_length;
    for (name_length = 0; buffer[name_length] != '='; name_length++) {
        if (buffer[name_length] == '\0') { errno = EINVAL; return -1; }
        if (name_length >= MAX_PUT_NAME_SIZE) { errno = ENAMETOOLONG; return -1; }
        name[name_length] = buffer[name_length];
    }
    if (name_length == 0) {errno = EINVAL; return -1;}
    name[name_length] = '\0';

    const char * result;

    // Try cache O(1)
    if (*cache >= 0 && *cache < env_count) {
        result = env_helper(name, cache, *cache);
        if (result != NULL) {
            environ[*cache] = buffer;
            return 0; 
        }
    } 

    // Try all O(N)
    for (size_t i = 0; environ[i] != NULL; i++) {
        result = env_helper(name, cache, i);
        if (result != NULL) {
            environ[*cache] = buffer;
            return 0; 
        }
    }

    // Insert
    if (env_count < MAX_ENV_VARS) {
        environ[env_count] = buffer;
        env_count++;
        environ[env_count] = NULL;
        return 0;
    }

    errno = ENOMEM;
    return -1;
}
