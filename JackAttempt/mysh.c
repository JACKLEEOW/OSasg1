#include "cstring.h"
#include "jobs.h"


int main(int argc, char *argv[], char *envp[])
{
  int exitShell = 0;
  struct Command cmd;
  run_command(&cmd,envp);
  


  /* TO DO: prompt for and read command line */
  

  while (!exitShell)
    {
      /* TO DO: process command line */
      /* TO DO: prompt for and read command line */
    }

  return 0;
}