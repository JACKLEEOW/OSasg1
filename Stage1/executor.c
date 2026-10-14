#include "executor.h"
#include "const.h"
#include "cstring.h"
#include "parser.h"
#include "path.h"
#include <unistd.h>
#include "env.h"
#include <sys/wait.h>
#include <fcntl.h>

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
static int redirect_fd (int fd, int target_fd){
    int redirect_status = dup2(fd, target_fd);
    
    if (redirect_status == -1) {
        write_error("redirect failed");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

static int redirect_input(const char* path) {
    int fd = open(path,O_RDONLY);
    if (fd ==-1) {
        write_error("cannot open input file");
        return -1;
    }
    return redirect_fd(fd,STDIN_FILENO);
}

static int redirect_output(const char* path, int append) {
    int flags = O_WRONLY | O_CREAT;
    if (append) {
        flags = flags | O_APPEND;
    }
    else {
        flags = flags | O_TRUNC;
    }
    int fd = open(path,flags,0644);

    if (fd ==-1) {
        write_error("cannot open output file");
        return -1;
    }
    return redirect_fd(fd, STDOUT_FILENO);
}

static int apply_redirects(const Pipeline *pipeline){
    if (pipeline->infile != NULL) {
        if (redirect_input(pipeline->infile) == -1) {
            return -1;
        }
    } 
    if (pipeline-> outfile != NULL) {
        if (redirect_output(pipeline->outfile, pipeline->append) == -1)
        return -1;
    }
    return 0;
}
// child only: replaces this process with the command, never returns
static void exec_child(Command *command) {
    execvpe(command->argv[0], command->argv, (char * const *) environ);
    write_error("command not found"); // only reached if execvpe failed
    _exit(1);
}

// forks and runs the command in the child, returns the child's pid or -1 on failure
static pid_t spawn_command(Command *command, const Pipeline *pipeline) {
    pid_t pid = fork();
    if (pid == -1) {
        write_error("fork failed");
        return -1;
    }
    if (pid == 0) {
        if (apply_redirects(pipeline) == -1) {
            _exit(1);
        }
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

static void close_all_pipes (int pipes[][2], int count) {
    for (int i = 0; i < count; i ++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

static int create_pipes(int pipes[][2],int count) {
    for (int i = 0; i < count; i++) {
        if (pipe(pipes[i]) == -1) {
            write_error("pipe failed");
            close_all_pipes(pipes,i);
            return -1;
        }
    }
    return 0;
}
// points the stage's stdin/stdout at the neighboring pipes, return 0 or -1 on fail
static int connect_stage_pipes(int pipes[][2], int stage_index, int num_stages) {
    // if not first stage stdin comes from prev pipe read
    if (stage_index > 0) {
        if (dup2(pipes[stage_index - 1][0], STDIN_FILENO) == -1) {
            write_error("pipe redirect failed");
            return -1;
        }
    }
    // if not last, std out goes into this pipes write end
    if (stage_index < num_stages - 1) {
        if (dup2(pipes[stage_index][1], STDOUT_FILENO) == -1) {
            write_error("pipe redirect failed");
            return -1;
        }
    }
    return 0;
}

// applies < to the first stage and > / >> to the last stage, return 0 or -1
static int apply_stage_redirects(const Pipeline *pipeline, int stage_index) {
    // first stage
    if (stage_index == 0 && pipeline->infile != NULL) {
        if (redirect_input(pipeline->infile) == -1) {
            return -1;
        }
    }
    // last stage
    if (stage_index == pipeline->num_stages - 1 && pipeline->outfile != NULL) {
        if (redirect_output(pipeline->outfile, pipeline->append) == -1) {
            return -1;
        }
    }
    return 0;
}

// sets up all of one stages input and output, then close the leftover pipe fds
static int setup_stage_io(const Pipeline *pipeline, int stage_index, int pipes[][2]) {
    if (connect_stage_pipes(pipes, stage_index, pipeline->num_stages) == -1) {
        return -1;
    }
    if (apply_stage_redirects(pipeline, stage_index) == -1) {
        return -1;
    }
    close_all_pipes(pipes, pipeline->num_stages - 1);
    return 0;
}

static pid_t spawn_stage(Pipeline *pipeline, int stage_index, int pipes[][2]){
    pid_t pid = fork();
    if (pid == -1) {
        write_error("fork failure");
        return -1;
    }
    if (pid == 0) {
        if (setup_stage_io(pipeline,stage_index,pipes)== -1) {
            _exit(1);
        }
        exec_child(&pipeline->stages[stage_index]);
    }
    return pid;
}
// fork every stage in order, returns how many stages started 
static int spawn_all_stages (Pipeline *pipeline, int pipes[][2], pid_t pids[]){
    for (int i = 0; i < pipeline->num_stages; i++){
        pids[i] = spawn_stage(pipeline,i,pipes);
        if (pids[i] == -1) {
            return i;
        }
        
    }
    return pipeline->num_stages;
}
// wait for the first count children, return EXE_ERROR if any wait failed
static ExecutorStatus wait_for_all(pid_t pids[], int count) {
    ExecutorStatus result = EXE_OK;
    for (int i = 0; i < count; i++) {
        if (wait_for_child(pids[i]) == EXE_ERROR) {
            result = EXE_ERROR;
        }
    }
    return result;
}
ExecutorStatus execute_pipeline(Pipeline *pipeline){
    if (pipeline->num_stages > MAX_PIPELINE_STAGES) {
        write_error("too many commands in pipeline");
        return EXE_ERROR;
    }
    int pipes[MAX_PIPELINE_STAGES -1][2];
    pid_t pids [MAX_PIPELINE_STAGES];
    if (pipeline->num_stages == 0 || pipeline->stages[0].argc == 0) {
        return EXE_OK;
    }
    if (is_builtin(pipeline)) {
        return run_builtin(pipeline);
    }
    int pipe_count = pipeline-> num_stages - 1;
    if (create_pipes(pipes,pipe_count) == -1) {
        return EXE_ERROR;
    }


    int started = spawn_all_stages(pipeline, pipes,pids);
    close_all_pipes(pipes,pipe_count); // parent closes copies before waiting
    if (started != pipeline->num_stages) {
        return EXE_ERROR;
    }
    if (pipeline->background) { // for later
        return EXE_OK;
    }
    ExecutorStatus result = wait_for_all(pids,started);
    return result;

}
