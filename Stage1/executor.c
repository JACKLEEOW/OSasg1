#include "executor.h"
#include "builtin.h"
#include "io.h"
#include "const.h"
#include "cstring.h"
#include "parser.h"
#include "path.h"
#include <unistd.h>
#include "env.h"
#include <sys/wait.h>
#include <fcntl.h>

// returns 1 if the pipeline is a single exit or cd command, which must run in the shell itself
static int is_builtin(const Pipeline *pipeline) {
    if (pipeline->num_stages != 1 || pipeline->stages[0].argv[0] == NULL) {
        return 0;
    }
    const char *name = pipeline->stages[0].argv[0];
    return c_strcmp(name, "exit") == 0 || c_strcmp(name, "cd") == 0;
}

// runs exit or cd in the parent, call only after is_builtin returned 1
static ExecutorStatus run_builtin(Pipeline *pipeline) {
    Command *command = &pipeline->stages[0];
    const char **argv = (const char **) command->argv;
    int status;
    if (c_strcmp(argv[0], "exit") == 0) {
        status = exit_builtin(argv, command->argc);
    } else {
        status = cd_builtin(argv, command->argc);
    }
    return status == 0 ? EXE_OK : EXE_ERROR;
}
static int redirect_fd (int fd, int target_fd){
    int redirect_status = dup2(fd, target_fd);
    
    if (redirect_status == -1) {
        WRITE_ERR(ERROR_START " redirect failed\n");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

static int redirect_input(const char* path) {
    int fd = open(path,O_RDONLY);
    if (fd ==-1) {
        WRITE_ERR(ERROR_START " cannot open input file\n");
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
        WRITE_ERR(ERROR_START " cannot open output file\n");
        return -1;
    }
    return redirect_fd(fd, STDOUT_FILENO);
}

// child only: replaces this process with the command, never returns
static void exec_child(Command *command) {
    execvpe(command->argv[0], command->argv, (char * const *) environ);
    WRITE_ERR(ERROR_START " command not found\n"); // only reached if execvpe failed
    _exit(1);
}

// blocks until the child finishes, reports an error if the wait itself fails
static ExecutorStatus wait_for_child(pid_t pid) {
    if (waitpid(pid, NULL, 0) == -1) {
        WRITE_ERR(ERROR_START " process failed\n");
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
            WRITE_ERR(ERROR_START " pipe failed\n");
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
            WRITE_ERR(ERROR_START " pipe redirect failed\n");
            return -1;
        }
    }
    // if not last, std out goes into this pipes write end
    if (stage_index < num_stages - 1) {
        if (dup2(pipes[stage_index][1], STDOUT_FILENO) == -1) {
            WRITE_ERR(ERROR_START " pipe redirect failed\n");
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
        WRITE_ERR(ERROR_START " fork failure\n");
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
        WRITE_ERR(ERROR_START " too many commands in pipeline\n");
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
