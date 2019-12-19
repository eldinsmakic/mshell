/* mshell - a job manager */

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "jobs.h"
#include "cmd.h"

/*
* do_help - Show all the available commands
*/
void do_help() {
    printf("available commands are:\n");
    printf(" exit - cause the shell to exit\n");
    printf(BOLD "\t exit\n" NORM);
    printf(" jobs - display status of jobs in the current session\n");
    printf(BOLD "\t jobs\n" NORM);
    printf(" fg   - run a job identified by its pid or job id in the foreground\n");
    printf(BOLD "\t fg " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" bg   - run a job identified by its pid or job id in the background\n");
    printf(BOLD "\t bg " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" stop - stop a job identified by its pid or job id\n");
    printf(BOLD "\t stop " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" kill - kill a job identified by its pid or job id\n");
    printf(BOLD "\t kill " NORM "pid" BOLD "|" NORM "jobid \n");
    printf(" help - print this message\n");
    printf(BOLD "\t help\n" NORM);
    printf("\n");
    printf("ctrl-z and ctrl-c can be used to send a SIGTSTP and a SIGINT, respectively\n\n");
}

/* 
* treat_argv - Determine pid or jobid and return the associated job structure 
*/
struct job_t * treat_argv(char **argv) {
    struct job_t *jobp = NULL;

    /* Ignore command if no argument */
    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return NULL;
    }

    /* Parse the required PID or %JID arg */
    if (isdigit((int) argv[1][0])) {
        pid_t pid = atoi(argv[1]);
        if (!(jobp = jobs_getjobpid(pid))) {
            printf("(%d): No such process\n", (int) pid);
            return NULL;
        }
    } else if (argv[1][0] == '%') {
        int jid = atoi(&argv[1][1]);
        if (!(jobp = jobs_getjobjid(jid))) {
            printf("%s: No such job\n", argv[1]);
            return NULL;
        }
    } else {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return NULL;
    }

    return jobp;
}


/* 
* do_bg - Emulate the builtin bg command 
*/
void do_bg(char **argv)
{
  pid_t pid;
  struct job_t * job;
  if (verbose)
       printf("do_bg: entering\n");
  job = treat_argv(argv);
  assert(job != NULL);
  pid = job->jb_pid;
  kill(pid,SIGCONT);
  job->jb_state = BG;
  if (verbose)
       printf("do_bg: exiting\n");
  return;
}

/* 
* waitfg - Block until process pid is no longer the foreground process 
*/
void waitfg(pid_t pid)
{
  struct job_t * fg_job;
  if (verbose)
       printf("waitfg: entering\n");
  fg_job =  jobs_getjobpid(pid);
  assert(fg_job != NULL);

  while (fg_job->jb_state == FG)
  {
    if(verbose)
      printf("WAITFG : RUNNING\n");
    sleep(1);
  }
  if (verbose)
       printf("waitfg: exiting\n");

}

/* 
* do_fg - Emulate the builtin fg command 
*/
void do_fg(char **argv)
{

  pid_t pid;
  struct job_t * job;
  if (verbose)
       printf("do_fg: entering\n");
  job = treat_argv(argv);
  assert(job != NULL);
  pid = job->jb_pid;
  kill(pid,SIGCONT);
  job->jb_state = FG;
  waitfg(pid);
  if (verbose)
       printf("do_fg: exiting\n");
  return;
}

/* 
* do_stop - Emulate the builtin stop command 
*/
void do_stop(char **argv)
{
  pid_t pid;
  struct job_t * job;
  if (verbose)
       printf("do_stop: entering\n");
  job = treat_argv(argv);
  assert(job != NULL);
  pid = job->jb_pid;
  if (job->jb_state == BG)
    kill(pid,SIGSTOP);
  job->jb_state = ST;
  if (verbose)
       printf("do_stop: exiting\n");
  return;
}

/*
* do_kill - Emulate the builtin kill command 
*/
void do_kill(char **argv)
 {
  pid_t pid;
  struct job_t * job;
  if (verbose)
        printf("do_stop: entering\n");
  job = treat_argv(argv);
  assert(job != NULL);
    pid = job->jb_pid;
  if (job->jb_state == BG)
    kill(pid,SIGKILL);
  if (verbose)
       printf("do_stop: exiting\n");
  return;
}

/* 
* do_exit - Emulate the builtin exit command 
*/
void do_exit()
{
  int i,length;
  pid_t pid;
  struct job_t * job;
  if (verbose)
      printf("DO_EXIT : GOING IN\n");
  length = jobs_maxjid();
  job = jobs_getstoppedjob();
  while( job != NULL)
  {
    pid = job->jb_pid;
    job->jb_state = BG;
    kill(SIGCONT,pid);
    job = jobs_getstoppedjob();
  }
  if (verbose)
      printf("DO_EXIT : BETWEEN\n");
  for(i=0;i < length ; i++)
    {
        job = jobs_getjobjid(i);
        if (job != NULL)
        {
          pid = job->jb_pid;
          kill(SIGTERM,pid);
        }
    }
  for(i=0;i < length ; i++)
    {
        job = jobs_getjobjid(i);
        if (job != NULL)
        {
          pid = job->jb_pid;
          kill(SIGKILL,pid);
        }
    }
    if (verbose)
        printf("DO_EXIT : EXIT IN\n");
    exit(0);

}

/* 
* do_jobs - Emulate the builtin jobs command 
*/
void do_jobs()
 {
   if (verbose) {
       printf("DO_JOBS : GOING IN\n");
     }
     jobs_listjobs();
     printf("\n" );

     if (verbose) {
       printf("DO_JOBS : EXITING\n");
     }

    return;
}
