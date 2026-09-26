#include <unistd.h>
#include "jobs.h"
#include "const.h"
#include <sys/wait.h>
#include "cstring.h"
#define ERROR "ERROR"
#define CHILD_MSG "im child\n"
#define PARENT_MSG "im parent\n"


void run_command(struct Command *cmd, char *envp[]) {
    
    pid_t pid = fork(); // 0 is child, >0 parent -1 is fail no child

    if(pid == -1) {
        write(STDERR_FILENO,ERROR,c_strlen(ERROR));
        return;
    }
    if(pid == 0) {
        write(STDOUT_FILENO,CHILD_MSG, c_strlen(CHILD_MSG));
    }
    else if(pid > 0 ) {
        write(STDOUT_FILENO, PARENT_MSG, c_strlen(PARENT_MSG));
    }
    return;

}



