/* mshell - a job manager */

#ifndef _PIPE_H_
#define _PIPE_H_

#include "common.h"

/*  do_pipe - Emulate the builtin pipe "|" command */
extern void do_pipe(char *cmds[MAXCMDS][MAXARGS], int nbcmd, int bg);

#endif
