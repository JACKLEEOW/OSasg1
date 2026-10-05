#ifndef ENV_H
#define ENV_H

// #define MAX_ENV_VARS 256
// #define MAX_POOL 8192


extern char ** environ;

// void steal_and_impersonate_env();

const char * get_env(const char * name, int * returning_cache);

int put_env(const char * name, const char * buffer, int * returning_cache);

// const char * set_env();

#endif