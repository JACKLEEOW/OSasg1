#ifndef BUILTIN_H
#define BUILTIN_H

/* --------------------------- FUNCTION PROTOTYPES -------------------------- */

int exit_builtin(const char * argv[], int argc);

int cd_builtin(const char * argv[], int argc);

int kill_builtin(const char * argv[], int argc);

int jobs_builtin(const char * argv[], int argc);

#endif