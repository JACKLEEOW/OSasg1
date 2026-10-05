#include "env.h"
#include "cstring.h"
#include <stddef.h>
#include <stdio.h>

// static char env_pool[MAX_POOL + 1];
// static char * env_table[MAX_ENV_VARS + 1];

// void steal_and_impersonate_env() {
//     int free_pool_offset = 0;
//     int old_pool_offset = 0;

//     for (int i = 0; environ[i] != NULL || i > MAX_ENV_VARS; i++) {
//         old_pool_offset = free_pool_offset;
//         const char * s = environ[i];
//         int j = 0;
        
//         do {
//             env_pool[free_pool_offset++] = s[j];
//         } while (s[j++] != '\0' && free_pool_offset != MAX_POOL);
        
//         env_pool[free_pool_offset++] = '\0';
//         env_table[i] = old_pool_offset;
//         env_table[i + 1] = NULL;
//         if (free_pool_offset == MAX_POOL + 1) return;
//     }

//     environ = env_table;
// }

// const char * get_env(const char * name) {
//     int x;
//     return get_env(name, x);
// }

static const char * get_env_helper(const char * name, int * return_cache) {

    for (int i = 0; environ[i] != NULL; i++) {
        const char * s = environ[i];

        int j = 0;
        while (name[j] == s[j]) j++;
        if (name[j] == '\0' && s[j] == '=') {
            (*return_cache) = i;
            return (s + j + 1);
        }
    }

    *return_cache = -1;
    return NULL;
}

const char * get_env(const char * name, int * returning_cache) {

    if (returning_cache != NULL) {
        const char * s = environ[*returning_cache];

        int j = 0;
        while (name[j] == s[j]) j++;

        if (name[j] == '\0' && s[j] == '=') {
            return (s + j + 1);
        }
    }

    return get_env_helper(name, returning_cache);
}

int put_env(const char * name, const char * buffer, int * returning_cache) {
    return 0;
}
