/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>

#include "jelio.h"
#include "jelio_internal.h"

int ipcwait(struct ipc_stat *status, int *opt_exit_status)
{
  int s, es;
  pid_t pid;
  
  /* wait for exit() by main program */
  //  buprintv(1, "ipcwait() waiting for %%\n", pid(status->pid));
  
  pid = waitpid(status->pid, &s, 0);
  //buprintv(1, "%% exited\n", pid(pid));
  if(pid != status->pid)
    {
      /* FIXME: handle all cases */
      buprintv(2, "ERR wrong child: %% should be %%\n",
	       pid(pid), pid(status->pid));
    }
  es = -1;
  if(WIFEXITED(s))
    es = WEXITSTATUS(s);
  
  if(opt_exit_status)
    *opt_exit_status = s;
  
  if(status->out.pid > 0)
    {
      //buprintv(1, "reading out from %%\n", pid(status->out.pid));
      status->out.size = jelio_read(status->out.fd, status->out.data, status->out.size, NULL);
      close(status->out.fd);
      //buprintv(1, "waiting for out %%\n", pid(status->out.pid));
      pid = waitpid(status->out.pid, NULL, 0);
      //buprintv(1, "%% exited\n", pid(pid));
    }

  if(status->err.pid > 0)
    {
      //buprintv(1, "reading err from %%\n", pid(status->err.pid));
      status->err.size = jelio_read(status->err.fd, status->err.data, status->err.size, NULL);
      close(status->err.fd);
      pid = waitpid(status->err.pid, NULL, 0);
    }
  
  return es;
}
