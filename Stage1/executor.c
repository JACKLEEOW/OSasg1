#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "executor.h"
#include "builtin.h"
#include "io.h"
#include "const.h"
#include "cstring.h"
#include "parser.h"
#include "path.h"
#include "env.h"
#include "job_control.h"
#include "signals.h"

/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

/** @brief Sets up process group ids */
static int setpgid_helper(pid_t incoming, pid_t group) {
    if (setpgid(incoming, group) > 0) {
        WRITE_ERR(ERROR_START " setpgid: ", strerror(errno), "\n");
        return -1;
    }

    return 0;
}

/** @brief returns 1 if the pipeline is a single exit or cd command, which must run in the shell itself */
static int is_builtin(const Pipeline *pipeline) {
    if (pipeline->num_stages != 1 || pipeline->stages[0].argv[0] == NULL) {
        return 0;
    }
    const char *name = pipeline->stages[0].argv[0];
    return c_strcmp(name, "exit") == 0 || c_strcmp(name, "cd") == 0 || c_strcmp(name, "jobs") == 0 || c_strcmp(name, "kill") == 0;
}

/** @brief Runs exit or cd in the parent, call only after is_builtin returned 1 */
static int run_builtin(Pipeline *pipeline) {
    Command *command = &pipeline->stages[0];
    const char **argv = (const char **) command->argv;
    int status;

    if (c_strcmp(argv[0], "exit") == 0) {
        status = exit_builtin(argv, command->argc);

    } else if (c_strcmp(argv[0], "jobs") == 0) {
        status = jobs_builtin(argv, command->argc);

    } else if (c_strcmp(argv[0], "kill") == 0) {
        status = kill_builtin(argv, command->argc);

    } else {
        status = cd_builtin(argv, command->argc);
    }

    return status;
}

/** @brief ... */
static int redirect_fd (int fd, int target_fd){
    int status = 0;
    
    if (dup2(fd, target_fd) < 0) {
        WRITE_ERR(ERROR_START " redirect: ", strerror(errno), "\n");
        status = -1;
    }

    if (close(fd) < 0) {
        WRITE_ERR(ERROR_START " redirect: ", strerror(errno), "\n");
        status = -1;
    }

    return status;
}

/** @brief ... */
static int redirect_input(const char* path) {
    int fd = open(path,O_RDONLY);

    if (fd < 0) {
        WRITE_ERR(ERROR_START " ", path, ": ", strerror(errno), "\n");
        return -1;
    }

    return redirect_fd(fd,STDIN_FILENO);
}

/** @brief ... */
static int redirect_output(const char* path, int append) {
    int flags = O_WRONLY | O_CREAT;
    if (append) flags = flags | O_APPEND;
    else flags = flags | O_TRUNC;

    int fd = open(path,flags,0644);

    if (fd < 0) {
        WRITE_ERR(ERROR_START " ", path, ": ", strerror(errno), "\n");
        return -1;
    }

    return redirect_fd(fd, STDOUT_FILENO);
}

/** @brief Child only: replaces this process with the command, never returns */ 
static void exec_child(Command *command) {
    execvpe(command->argv[0], (char * const *) command->argv, (char * const *) environ);
    WRITE_ERR(ERROR_START " ", command->argv[0], ": ", strerror(errno), "\n"); // only reached if execvpe failed
    _exit(1);
}

/** @brief ... */
static int close_all_pipes(int pipes[][2], int count) {
    int status = 0;

    for (int i = 0; i < count; i ++) {
        if (close(pipes[i][0]) < 0) {
            WRITE_ERR(ERROR_START " close: ", strerror(errno), "\n");
            status = -1;
        }

        if (close(pipes[i][1]) < 0) {
            WRITE_ERR(ERROR_START " close: ", strerror(errno), "\n");
            status = -1;
        }
    }

    return status;
}

/** @brief ... */
static int create_pipes(int pipes[][2],int count) {
    for (int i = 0; i < count; i++) {
        if (pipe(pipes[i]) < 0) {
            WRITE_ERR(ERROR_START " pipe: ", strerror(errno), "\n");
            close_all_pipes(pipes, i);
            return -1;
        }
    }

    return 0;
}

/** @brief Points the stage's stdin/stdout at the neighboring pipes, return 0 or -1 on fail */
static int connect_stage_pipes(int pipes[][2], int stage_index, int num_stages) {
    
    // If not first stage stdin comes from prev pipe read
    if (stage_index > 0) {
        if (dup2(pipes[stage_index - 1][0], STDIN_FILENO) < 0) {
            WRITE_ERR(ERROR_START " pipe redirect: ", strerror(errno), "\n");
            return -1;
        }
    }

    // If not last, std out goes into this pipes write end
    if (stage_index < num_stages - 1) {
        if (dup2(pipes[stage_index][1], STDOUT_FILENO) < 0) {
            WRITE_ERR(ERROR_START " pipe redirect: ", strerror(errno), "\n");
            return -1;
        }
    }

    return 0;
}

/** @brief Applies < to the first stage and > / >> to the last stage, return 0 or -1 */
static int apply_stage_redirects(const Pipeline *pipeline, int stage_index) {
    
    // First stage
    if (stage_index == 0 && pipeline->infile != NULL) {
        if (redirect_input(pipeline->infile) < 0) return -1;
    }

    // Last stage
    if (stage_index == pipeline->num_stages - 1 && pipeline->outfile != NULL) {
        if (redirect_output(pipeline->outfile, pipeline->append) < 0) return -1;
    }

    return 0;
}

/** @brief Sets up all of one stages input and output, then close the leftover pipe fds */
static int setup_stage_io(const Pipeline *pipeline, int stage_index, int pipes[][2]) {

    if (connect_stage_pipes(pipes, stage_index, pipeline->num_stages) < 0) return -1;

    if (apply_stage_redirects(pipeline, stage_index) < 0) return -1;

    close_all_pipes(pipes, pipeline->num_stages - 1);

    return 0;
}

/** @brief ... */
static pid_t spawn_stage(Pipeline *pipeline, int stage_index, int pipes[][2], pid_t pids[]){
    pid_t pid = fork();
    if (pid < 0) {

        WRITE_ERR(ERROR_START " fork: ", strerror(errno), "\n");
        return pid;

    } else if (pid == 0) {

        pid_t parent_pid = (stage_index == 0) ? 0 : pids[0];

        if (restore_children_default_signals() < 0) _exit(1);

        if (setpgid_helper(0, parent_pid) < 0) _exit(1); 

        if (setup_stage_io(pipeline, stage_index,pipes) < 0) _exit(1);

        exec_child(&pipeline->stages[stage_index]);
    }

    return pid;
}

/** @brief Fork every stage in order, returns how many stages started */
static int spawn_all_stages (Pipeline *pipeline, int pipes[][2], pid_t pids[]){
    for (int i = 0; i < pipeline->num_stages; i++){
        pids[i] = spawn_stage(pipeline,i,pipes, pids);

        if (pids[i] < 0) return i;
    
        if (setpgid_helper(pids[i], pids[0]) < 0) return i;
    }

    return pipeline->num_stages;
}

/** @brief Wait for the first count children, return EXE_ERROR if any wait failed */
static int wait_for_all(Pipeline *pipeline, pid_t pids[], int count) {
    pid_t shell_pid = getpgrp();
    int status;
    pid_t pgid = pids[0];
    if (pgid > 0) {
        Job *job = add_job(pgid, count, !pipeline->background);
        if (!pipeline->background) {
            
            // Hand terminal control to child process
            tcsetpgrp(STDIN_FILENO, pgid);
            
            while (job->state == JOB_RUNNING) {
                pid_t w = waitpid(-pgid, &status, WUNTRACED);
                if (w < 0) {
                    if (errno == EINTR) continue;
                    job->state = JOB_DONE;
                    break;                      
                }
            
                if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT) WRITE_OUT("\n");

                update_job_status(job, status);
            }

            // Take back terminal control
            tcsetpgrp(STDIN_FILENO, shell_pid);

            if (job->state == JOB_STOPPED) {
                job->foreground = 0;
                WRITE_OUT("\n");
                write_job(job);
            } else if (job->state == JOB_DONE) {
                remove_job(job);
            }

        } else {
            // Display new background process to user
            write_job(job);
        }

        return 0;
    }

    return -1;
}

/* --------------------------- FUNCTION DEFINITONS -------------------------- */

ExecutorStatus execute_pipeline(Pipeline *pipeline){
    if (pipeline->num_stages > MAX_PIPELINE_STAGES) {
        WRITE_ERR(ERROR_START " Too many commands in pipeline\n");
        return EXE_ERROR;
    }

    int pipes[MAX_PIPELINE_STAGES -1][2];
    pid_t pids [MAX_PIPELINE_STAGES];

    if (pipeline->num_stages == 0 || pipeline->stages[0].argc == 0) {
        return EXE_OK;
    }

    if (is_builtin(pipeline)) {
        return (run_builtin(pipeline) < 0) ? EXE_ERROR : EXE_OK;
    }

    if (is_jobs_full()) {
        WRITE_ERR(ERROR_START " Too many jobs\n");
        return EXE_ERROR;
    }

    int pipe_count = pipeline-> num_stages - 1;
    if (create_pipes(pipes,pipe_count) == -1) {
        return EXE_ERROR;
    }


    int started = spawn_all_stages(pipeline, pipes, pids);
    close_all_pipes(pipes, pipe_count); // parent closes copies before waiting
    if (started != pipeline->num_stages) {
        return EXE_ERROR;
    }
    
    int result = wait_for_all(pipeline, pids, started);
    return (result < 0) ? EXE_ERROR : EXE_OK;

}
