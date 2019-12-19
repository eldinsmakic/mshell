/* mshell - a job manager */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "assert.h"
#include "common.h"
#include "pipe.h"
#include "jobs.h"
#include "sighandlers.h"
#include "cmd.h"
#define BUFSIZE 100

/*
*  do_pipe - Emulate the builtin pipe "|" command 
*/
void do_pipe(char *cmds[MAXCMDS][MAXARGS], int nbcmd, int bg)
{
      int fd[2];
      int i;
      int input;
      pid_t pid, pgid;
      input = STDIN_FILENO;
      pgid = 0;
      for(i=0; i < nbcmd; i++)
      {
          assert(pipe(fd) != -1);
          switch(pid = fork())
          {
              case -1:
                  unix_error("fork error");
                  break;
              case 0:

                  dup2(input, STDIN_FILENO);
                  if(i < nbcmd - 1)
                      dup2(fd[1], STDOUT_FILENO);
                  close(fd[0]);
                  execvp(cmds[i][0], cmds[i]);
                  assert(0);
              default:
                  close(fd[1]);
                  input = fd[0];
                  pgid = pid;
          }
      }
      close(fd[0]);
      assert(jobs_addjob(pid, (bg == 1 ? BG : FG), cmds[0][0]) == 1);
      printf("pid %d job pid %d\n",getpgid(pid), jobs_fgpid());
      if (!bg)
          waitfg(pid);
      return ;
}
