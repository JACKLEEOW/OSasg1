#ifndef JOBS_H
#define JOBS_H

#define MAX_ARGS 16     /* TO DO */

struct Command
{
  char *argv[MAX_ARGS+1];
  unsigned int argc;
};

void run_command(struct Command *cmd, char* envp[]);

#endif