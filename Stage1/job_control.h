#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

#include <sys/types.h>
#include <sys/wait.h>

/* --------------------------------- MACROS --------------------------------- */

#define MAX_JOBS 64

/* ---------------------------- STRUCTS AND ENUMS --------------------------- */

typedef enum { 
    JOB_RUNNING, 
    JOB_STOPPED, 
    JOB_DONE 
} JobState;

typedef struct {
    int id;                 // Job id
    pid_t pgid;             // Process group id
    JobState state;         // Job state
    int foreground;         // Foreground flag
    int live;               // Non-terminated processes remaining in pgid
    int stopped;            // Stopped processes in pgid
} Job;

/* --------------------------- FUNCTION PROTOTYPES -------------------------- */

// Get

Job *focused_job(); 
Job *get_job(int id);
Job *find_job(pid_t pgid);
int is_jobs_full();

// Add / Remove

Job *add_job(pid_t pgid, int stage_count, int fg);
void remove_job(Job *job);

// Updates

int set_focused_job(int id);
int update_job_status(Job *job, int status);

// Writes

void write_job(Job *job);
void display_jobs();

// Zombie Cleanup

void reap_jobs();

#endif