/* mshell - a job manager */

#ifndef _SIGHANDLERS_H_
#define _SIGHANDLERS_H_

typedef void handler_t(int);

/*
 * wrapper for the sigaction function
 */
extern int sigaction_wrapper(int signum, handler_t * handler);

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie childre n
 */
extern void sigchld_handler(int sig);

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
extern void sigint_handler(int sig);

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
extern void sigtstp_handler(int sig);

extern void sigquit_handler(int sig);

#endif
