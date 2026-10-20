#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "job_control.h"
#include "const.h"
#include "io.h"
#include "cstring.h"

/* --------------------------------- GLOBALS -------------------------------- */

static Job job_array[MAX_JOBS];
static int job_count = 0;
static int focused_job_id = -1;

/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

/** @brief Get the smallest unused id */
static int get_next_id() {
    int ids[MAX_JOBS];
    for (int i = 0; i < job_count; i++) { ids[i] = job_array[i].id; }

    // Cycle sort

    for (int i = 0; i < job_count; i++) {
        while ((1 <= ids[i] && ids[i] <= job_count) && (ids[ids[i] - 1] != ids[i])) {
            int target_idx = ids[i] - 1;
            int temp_id = ids[i];
            ids[i] = ids[target_idx];
            ids[target_idx] = temp_id;
        }
    }

    for (int i = 0; i < job_count; i++) {
        if (ids[i] != i + 1) return i + 1;
    }

    return job_count + 1;
}

/* --------------------------- FUNCTION DEFINITONS -------------------------- */

// Get

int is_jobs_full() {
    return (job_count == MAX_JOBS);
}

Job *focused_job() {
    return get_job(focused_job_id);
}

Job *get_job(int id) {
    for (int i = 0; i < job_count; i++) {
        Job *curr_job = &job_array[i];
        if (curr_job->id == id) return curr_job;
    }
    return NULL;
}

Job *find_job(pid_t pgid) {
    for (int i = 0; i < job_count; i++) {
        Job *curr_job = &job_array[i];
        if (curr_job->pgid == pgid) return curr_job;
    }
    return NULL;
}

// Add / Remove

Job *add_job(pid_t pgid, int stage_count, int fg) {
    if (job_count == MAX_JOBS) return NULL; // You messed up badly

    Job *job = &job_array[job_count];
    job->id = get_next_id();
    job->pgid = pgid;
    job->foreground = fg;
    job->live = stage_count;
    job->stopped = 0;
    job->state = JOB_RUNNING;

    focused_job_id = job->id;

    job_count++; // Cannot increment before get_next_id

    return job;
}

void remove_job(Job *job) {
    int i;
    for (i = 0; i < job_count; i++) {
        Job *curr_job = &job_array[i];
        if (curr_job->pgid == job->pgid) {
            if (curr_job->id == focused_job_id) focused_job_id = -1;
            break;
        };
    }

    if (i == job_count) return;

    for (; i < job_count - 1; i++) {
        job_array[i] = job_array[i + 1];
    }

    job_count--;
    return;
}

// Updates

int set_focused_job(int id) {
    Job *job = get_job(id);
    if (job != NULL) {
        focused_job_id = id;
        return 0;
    }
    return -1;
}

int update_job_status(Job *job, int status) {
    if (job == NULL) return -1;

    // Screw this, I am probably missing other cases
    // Also screw this 
    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        job->live--; // Process was killed
    } else if (WIFSTOPPED(status)) {
        job->stopped++; // Process was paused
    } else if (WIFCONTINUED(status)) {
        job->stopped--; // Process was un-paused
    }

    if (job->live == 0) {
        job->state = JOB_DONE;
    } else if (job->stopped == job->live) {
        job->state = JOB_STOPPED;
    } else {
        job->state = JOB_RUNNING;
    }                           

    return 0;
}

// Writes

void write_job(Job *job) {
    const char *focused_text = (job->id == focused_job_id) ? "+" : " ";
    const char *state_text;
    switch (job->state) {
    case JOB_RUNNING:
        state_text = "Running";
        break;
    case JOB_DONE:
        state_text = "Done";
        break;
    case JOB_STOPPED:
        state_text = "Stopped";
        break;
    default:
        state_text = "Error";
        break;
    }

    char buffer[3];
    const char *id_text = uint_to_cstr(buffer, 3, (unsigned int)job->id);

    char buffer_pgid[10];
    const char *pgid_text = uint_to_cstr(buffer_pgid, 10, (unsigned int)job->pgid);
    WRITE_OUT(MYSH_START " [",id_text,"]",focused_text, " (",pgid_text,") -> ", state_text, "\n");
}

void display_jobs() {
    reap_jobs();
    for (int i = 0; i < job_count; i++) {
        Job *curr_job = &job_array[i];

        // Reap already displays done jobs
        if (curr_job->state != JOB_DONE)
            write_job(curr_job);
    }
}

// Zombie Cleanup

void reap_jobs() {
    for (int i = job_count - 1; i >= 0; i--) {
        Job *curr_job = &job_array[i];

        // if (curr_job->state == JOB_DONE) remove_job(curr_job);

        // Not sure if this is actually need
        if (curr_job->foreground == 1) continue;

        int status;
        pid_t w;

        while ((w = waitpid(-curr_job->pgid, &status, WNOHANG | WUNTRACED)) > 0) {
            update_job_status(curr_job, status);
        }

        // ECHILD no children to collect in pgid, already reaped
        if (w < 0 && errno == ECHILD && curr_job->state != JOB_STOPPED) {
            curr_job->state = JOB_DONE;
        } else if (w < 0) {
            WRITE_ERR(ERROR_START " Failed reap: ", strerror(errno), "\n");
        }
        
        // Output if the job is done
        if (curr_job->state == JOB_DONE) {
            write_job(curr_job);
            remove_job(curr_job);
        } 
    }
}
   