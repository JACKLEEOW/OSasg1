#include "executor.h"
#include "const.h"
#include "cstring.h"
#include "parser.h"
#include "path.h"
#include <unistd.h>
#include "env.h"
#include <sys/wait.h>
static void write_error (const char *message) {
    if (message == NULL) {
        return;
    }
    write(STDERR_FILENO, message, c_strlen(message));
    write(STDERR_FILENO, "\n", 1);
}

static int home_env_cache = -1; // gonna default to home if input is cd

static ExecutorStatus run_cd(const Command *command) {
    const char *target = NULL;
    if (command->argc == 1) {
        target = get_env("HOME", &home_env_cache);
        if (target == NULL) {
            write_error("HOME not set");
            return EXE_ERROR;
        }
    }
    else if(command->argc == 2){
        target = command->argv[1];
    }
    else{
        write_error("too many arguments");
        return EXE_ERROR;
    }
    int path_status = path_exists(target);

    if (path_status!= 0) {
        write_error("path does not exist");
        return EXE_ERROR;
    }

    int cwd_status = change_cwd(target);
    if (cwd_status != 0) {
        write_error("change directory failed");
        return EXE_ERROR;
    }
    return EXE_OK;
}

// returns 1 if the command's first word matches name, 0 otherwise (also 0 if empty)
static int command_is(const Command *command, const char *name) {
    if (command->argc == 0) {
        return 0;
    }
    return c_strcmp(command->argv[0], name) == 0;
}

// returns 1 if the pipeline is a single exit or cd command, which must run in the shell itself
static int is_builtin(const Pipeline *pipeline) {
    if (pipeline->num_stages != 1) {
        return 0;
    }
    const Command *command = &pipeline->stages[0];
    return command_is(command, "exit") || command_is(command, "cd");
}

// runs exit or cd in the parent, call only after is_builtin returned 1
static ExecutorStatus run_builtin(const Pipeline *pipeline) {
    const Command *command = &pipeline->stages[0];
    if (command_is(command, "exit")) {
        return EXE_EXIT;
    }
    return run_cd(command);
}

// child only: replaces this process with the command, never returns
static void exec_child(Command *command) {
    execvpe(command->argv[0], command->argv, (char * const *) environ);
    write_error("command not found"); // only reached if execvpe failed
    _exit(1);
}

// forks and runs the command in the child, returns the child's pid or -1 on failure
static pid_t spawn_command(Command *command) {
    pid_t pid = fork();
    if (pid == -1) {
        write_error("fork failed");
        return -1;
    }
    if (pid == 0) {
        exec_child(command);
    }
    return pid;
}

// blocks until the child finishes, reports an error if the wait itself fails
static ExecutorStatus wait_for_child(pid_t pid) {
    if (waitpid(pid, NULL, 0) == -1) {
        write_error("process failed");
        return EXE_ERROR;
    }
    return EXE_OK;
}

ExecutorStatus execute_pipeline(Pipeline *pipeline){
    if (pipeline->num_stages == 0 || pipeline->stages[0].argc == 0) {
        return EXE_OK;
    }
    if (is_builtin(pipeline)) {
        return run_builtin(pipeline);
    }

    pid_t pid = spawn_command(&pipeline->stages[0]);
    if (pid == -1) {
        return EXE_ERROR;
    }
    return wait_for_child(pid);
}
