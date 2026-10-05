#ifndef ENV_H
#define ENV_H

#define MAX_ENV_VARS 256
// #define MAX_POOL 8192


extern const char ** environ;

void steal_and_impersonate_env();

const char * get_env(const char * name, int * cache);

int put_env(const char * name, const char * buffer, int * cache);

// const char * set_env();

#endif