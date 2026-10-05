#ifndef PATH_H
#define PATH_H

#define MAX_PATH 512

int set_cwd(const char * path);

const char * get_cwd();

int execvpe(const char *filename, char *const argv[], char *const envp[]);

int file_exists(const char * filepath);


#endif