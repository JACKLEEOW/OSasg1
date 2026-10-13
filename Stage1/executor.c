#include "executor.h"
#include "const.h"
#include "cstring.h"
#include "parser.h"
#include "path.h"
#include <unistd.h>
#include "env.h"

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

ExecutorStatus execute_pipeline(Pipeline *pipeline){
    if (pipeline->num_stages>= 1 
        && pipeline->stages[0].argc >= 1
        && c_strcmp(pipeline->stages[0].argv[0], "exit") == 0)
         {
            //check exit
        return EXE_EXIT;
    }
    //cd block
    if (pipeline->num_stages == 1
        && pipeline->stages[0].argc >= 1
        && c_strcmp(pipeline->stages[0].argv[0],"cd") == 0){
            return run_cd(&pipeline->stages[0]);
    }
    return EXE_OK;
}

