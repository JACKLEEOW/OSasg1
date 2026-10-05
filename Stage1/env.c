#include "env.h"
#include "cstring.h"
#include <stddef.h>
#include <stdio.h>

// static char env_pool[MAX_POOL + 1];
static const char * env_table[MAX_ENV_VARS + 1];
static int env_count = 0;
static int init = 0;

void steal_and_impersonate_env() {
    // int free_pool_offset = 0;
    // int old_pool_offset = 0;
    int i;
    for (i = 0; environ[i] != NULL || i > MAX_ENV_VARS; i++) {
        env_table[i] = environ[i];
        env_count++;
        // old_pool_offset = free_pool_offset;
        // const char * s = environ[i];
        // int j = 0;
        
        // do {
        //     env_pool[free_pool_offset++] = s[j];
        // } while (s[j++] != '\0' && free_pool_offset != MAX_POOL);
        
        // env_pool[free_pool_offset++] = '\0';
        // env_table[i] = old_pool_offset;
        // env_table[i + 1] = NULL;
        // if (free_pool_offset == MAX_POOL + 1) return;
    }

    env_table[i] = NULL;

    environ = env_table;
    init = 1;
}

// const char * get_env(const char * name) {
//     int x;
//     return get_env(name, x);
// }

static const char * env_helper(const char * name, int * return_cache, int i) {

    const char * env_var = environ[i];
    int j = 0;
    while (name[j] == env_var[j]) j++;
    if (name[j] == '\0' && env_var[j] == '=') {
        *return_cache = i;
        return (env_var + j + 1);
    }

    *return_cache = -1;
    return NULL;
}

const char * get_env(const char * name, int * cache) {
    if (init == 0) return NULL;
    if (name == NULL || cache == NULL) return NULL;
    const char * result;

    // try cache O(1)
    if (*cache >= 0 && *cache < env_count) {
        result = env_helper(name, cache, *cache);
        if (result != NULL) return result;
    } 

    // try all O(N)
    for (int i = 0; environ[i] != NULL; i++) {
        result = env_helper(name, cache, i);
        if (result != NULL) return result;
    }

    *cache = -1;
    return NULL;
}

int put_env(const char * name, const char * buffer, int * cache) {
    if (init == 0) return -1;
    if (buffer == NULL || cache == NULL) return -1;

    const char * result;

    // try cache O(1)
    if (*cache >= 0 && *cache < env_count) {
        result = env_helper(name, cache, *cache);
        if (result != NULL) {
            environ[*cache] = buffer;
            return 0; 
        }
    } 

    // try all O(N)
    for (int i = 0; environ[i] != NULL; i++) {
        result = env_helper(name, cache, i);
        if (result != NULL) {
            environ[*cache] = buffer;
            return 0; 
        }
    }

    // insert
    if (env_count < MAX_ENV_VARS) {
        environ[env_count] = buffer;
        env_count++;
        environ[env_count] = NULL;
    }


    return -1;
}
