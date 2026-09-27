#include "cstring.h"
#include "jobs.h"

int main(int argc, char *argv[], char *envp[])
{
  char test_cmd[] = "/bin/ls";
  int exitShell = 0;
  Command cmd;
  cmd.argv[0] = test_cmd;
  cmd.argv[1] = NULL;
  run_command(&cmd,envp);



  /* TO DO: prompt for and read command line */
  

  /*while (!exitShell)
    {
      /* TO DO: process command line */
      /* TO DO: prompt for and read command line 
    */ 

  return 0;
}