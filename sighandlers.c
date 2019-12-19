/* mshell - a job manager */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include "jobs.h"
#include "common.h"
#include "sighandlers.h"

/*
 * wrapper for the sigaction function
 */
int sigaction_wrapper(int signum, handler_t * handler)
{

  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = SA_RESTART;
  assert(sigemptyset(&sa.sa_mask) == 0);
  assert(sigaction(signum, &sa, NULL) == 0);
  return 1;

}


/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie childre n
 */
void sigchld_handler(int sig __attribute__((unused)))
 {
   int  status;
   pid_t child_pd;
   struct job_t * job_stop;
   if (verbose)
        printf("sigchld_handler: entering\n");
   child_pd = waitpid(-1,&status,WNOHANG|WUNTRACED);
   if(child_pd > 0)
      {
      job_stop = jobs_getjobpid(child_pd);
      if (job_stop == NULL)
            {
              if (verbose)
                  printf("sigchld_handler: exiting\n");
              return ;
            }
        if(WIFEXITED(status))
          {
            printf("Job n°%d (%d) terminated smoothly\n", job_stop->jb_jid, job_stop->jb_pid);
            jobs_deletejob(child_pd);
            if (verbose)
                printf("sigchld_handler: exiting\n");

            return;
          }
        if(WIFSTOPPED(status))
        {
          job_stop->jb_state = ST;
          printf("Job n°%d (%d) stopped by signal \n", job_stop->jb_jid, job_stop->jb_pid);
          if (verbose)
              printf("sigchld_handler: exiting\n");

          return;
        }
        else
        {
          printf("Job n°%d (%d) terminated badly\n", job_stop->jb_jid, job_stop->jb_pid);
          jobs_deletejob(child_pd);
          if (verbose)
              printf("sigchld_handler: exiting\n");

          return;
        }
      }



}
/*
* sigbasic_handler - The kernel sends a basic signal SIGINT OR SIGSTOP to the shell whenever
*     the user types ctrl-z  or ctrl-c at the keyboard. Catch it and suspend the
*     foreground job by sending it the signal.
*/
void sigbasic_handler(int sig)
{
  pid_t pid;
  if (verbose)
    printf("sigint_handler: entering\n");
  pid = jobs_fgpid();
  if (pid > 0)
  {
    kill(-pid, sig);
    if(verbose)
      printf("sigint_handler : kill done on %d\n", pid);
  }
  if (verbose)
    printf("sigint_handler: exiting\n");
  return;
}
/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void sigint_handler(int sig)
 {
   sigbasic_handler(sig);
 }

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig)
{
  sigbasic_handler(sig);
}
