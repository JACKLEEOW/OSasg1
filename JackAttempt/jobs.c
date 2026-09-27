#include <unistd.h>
#include "jobs.h"
#include "const.h"
#include <sys/wait.h>
#include "cstring.h"
#define ERROR "ERROR\n"
#define FORK_ERROR "FORK_ERROR\n"
#define CHILD_MSG "im child\n"
#define PARENT_MSG "im parent\n"
#define WAIT_ERROR "WAIT ERROR\n"


void run_command(Command *cmd, char *envp[]) {
    
    pid_t pid = fork(); // 0 is child, >0 parent -1 is fail no child

    if(pid == -1) {
        write(STDERR_FILENO,FORK_ERROR,c_strlen(FORK_ERROR));
        return;
    }
    if(pid == 0) {
        int exec = execve(cmd->argv[0], cmd->argv,envp);
        write(STDERR_FILENO,ERROR,c_strlen(ERROR));
        _exit(1);

    }
    else if(pid > 0 ) {
        int status; // for exit code of the child
        pid_t w = waitpid(pid,&status,0);
        if (w == -1) {
            // -1 means failed so will print an error
            write(STDERR_FILENO,WAIT_ERROR,c_strlen(WAIT_ERROR));

        }

    }
    return;

}



