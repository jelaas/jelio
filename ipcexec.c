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
#include <sysexits.h>

#include "jelio.h"
#include "jelio_internal.h"

static struct iocon iocon_none;
static char *const envp_empty[2] = { "PATH=/bin", NULL };
static char *const argv_empty[2] = { NULL, NULL };

/*
 FIXME: check that we dont leak fds to execve() program

*/

/* this is done in a worker process.
   since writing may block if noone reads the output.
   we create a child proc to do the writing.
   the child waits for the parent to exit by polling
   the end of a shared pipe. when the parent dies the pipe closes.
*/
static int _wr_worker(int ifd[2], char *buf, size_t size)
{
  struct pollfd pollfd[3];
  pid_t wpid;
  int hupfd[2], rc;
  
  pipe(hupfd);
  
  wpid=fork();
  if(wpid == 0)
    {
      close(1);
      close(2);
      close(ifd[0]);
      close(hupfd[1]);
      
      jelio_write(ifd[1], buf, size);
      close(ifd[1]);
      pollfd[0].fd = hupfd[0];
      pollfd[0].events = POLLIN;
      rc = poll(pollfd, 1, -1); /* wait for pipe to close */
      //buprintv(2, "poll rc == %%\n", d(rc));
      while(getppid()!=1)
	{
	  /* this loop should actually never be entered into!
	   */
	  rc = poll(NULL, 0, 200);
	  //buprintv(2, "ppid loop rc = %%\n", d(rc));
	}
      _exit(0);
    }
  close(hupfd[0]);
  return 0;
}


static int _ipc_child(struct iocon *in, struct iocon *out, struct iocon *err,
		      int ifd[2], int ofd[2], int efd[2])
{
  int fd;
  /* child */

  /* setup output for child */
  switch(out->type)
    {
    case IOCON_NONE:
      fd = open("/dev/null", O_RDWR, 0600);
      if(fd >= 0)
	{
	  dup2(fd, 1);
	  if(fd != 1) close(fd);
	}
      else
	{
	  if(pipe(ofd)==0)
	    {
	      dup2(ofd[1], 1);
	      if(ofd[1] != 1) close(ofd[1]);
	      close(ofd[0]);
	    }
	}
      break;
    case IOCON_MEM:
      //buprintv(1, "dup2(%%, 1)\n", d(ofd[1]));
      if(dup2(ofd[1], 1)==-1)
	{
	  close(1);
	  dup2(ofd[1], 1);
	  if(ofd[1] != 1) close(ofd[1]);
	}
      close(ofd[1]);
      break;
    case IOCON_CTRLFD:
      dup2(ofd[1], 1);
      if(ofd[1] != 1) close(ofd[1]);
      close(ofd[0]);
      break;
    case IOCON_PIPEFD:
      dup2(out->ep.pipefd, 1);
      break;
    }
  
  /* setup error output for child */
  switch(err->type)
    {
    case IOCON_NONE:
      fd = open("/dev/null", O_RDWR, 0600);
      if(fd >= 0)
	{
	  dup2(fd, 2);
	  if(fd != 2) close(fd);
	}
      else
	{
	  if(pipe(efd)==0)
	    {
	      dup2(efd[1], 2);
	      if(efd[1] != 2) close(efd[1]);
	      close(efd[0]);
	    }
	}
      break;
    case IOCON_MEM:
      if(dup2(efd[1], 2)==-1)
	{
	  close(2);
	  dup2(efd[1], 2);
	}
      close(efd[1]);
      break;
    case IOCON_CTRLFD:
      dup2(efd[1], 2);
      if(efd[1] != 2) close(efd[1]);
      close(efd[0]);
      break;
    case IOCON_PIPEFD:
      dup2(err->ep.pipefd, 2);
      break;
    }

  if(in->type == IOCON_MEM)
    _wr_worker(ifd, in->ep.buf, in->size); /* spawn worker process to supply input from memory */
  
  /* setup input for child */
  switch(in->type)
    {
    case IOCON_PIPEFD:
      if(dup2(in->ep.pipefd, 0)==-1)
	{
	  close(0);
	  if(dup2(in->ep.pipefd, 0)==-1)
	    _exit(2);
	}
      break;
    case IOCON_CTRLFD:
      dup2(ifd[0], 0);
      if(ifd[0] != 0) close(ifd[0]);
      close(ifd[1]);
      break;
    case IOCON_NONE:
      fd = open("/dev/null", O_RDWR, 0600);
      if(fd >= 0)
	{
	  dup2(fd, 0);
	  if(fd != 0) close(fd);
	}
      else
	{
	  if(pipe(ifd)==0)
	    {
	      dup2(ifd[0], 0);
	      close(ifd[1]);
	    }
	}
      break;
    case IOCON_MEM:
      if(dup2(ifd[0], 0)==-1)
	{
	  close(0);
	  dup2(ifd[0], 0);
	}
      close(ifd[1]);
      break;
    }
  return 0;
}

/*
 * Forks worker process for capturing output from child.
 */
static int _rd_worker(struct ipc_worker_data *data,
		      struct iocon *io, int fd[2])
{
  pid_t pid;
  int out[2];
  char *buf;
  ssize_t len;

  pipe(out);
  
  data->fd = out[0];
  data->data = io->ep.buf;
  data->size = io->size;

  pid = fork();
  if(pid == 0)
    {
      close(fd[1]);
      close(out[0]);
      
      /* read everything from fd into memory */
      buf = malloc(io->size);
      //buprintv(1, "reading at most %%\n", d(io->size));
      len = jelio_read(fd[0], buf, io->size, NULL);
      //buprintv(1, "read %% bytes\n", d(len));
      close(fd[0]);
      /* write everything from memory to out[1] */
      jelio_write(out[1], buf, len);
      _exit(0);
    }
  close(out[1]);
  data->pid = pid;
  return 0;
}

/*
 * Spawn a child process.
 * Configure stdin,stdout and stderr according to in,out,err.
 *
 * Problem cases: in(CTRLFD)+out(MEM) -- worker process with shared mmap ?
 *                in(MEM)+out(CTRLFD|PIPEFD) -- worker process with shared mmap ?
 */
struct ipc_stat *ipcexec(const char *fn, struct iocon *in, struct iocon *out, struct iocon *err, char *const *opt_argv, char *const *envp)
{
  pid_t pid;
  int ofd[2], efd[2], ifd[2];
  struct ipc_stat *status;
  char *const *argv = NULL;

  status = malloc(sizeof(struct ipc_stat));
  if(!status) return NULL;
  memset(status, 0, sizeof(struct ipc_stat));
  
  if(!in) in = &iocon_none;
  if(!out) out = &iocon_none;
  if(!err) err = &iocon_none;
  
  if(!opt_argv)
    {
      char **va;
      va = malloc(sizeof(char*)*2);
      if(!va) return NULL;
      va[0] = (char*) fn;
      va[1] = NULL;
      argv = va;
    }
  else
    argv = opt_argv;
  
  if(!envp) envp = envp_empty;

  switch(out->type)
    {
    case IOCON_CTRLFD:
      if(pipe(ofd)==-1) return status;
      out->ep.ctrlfd = ofd[0];
      break;
    case IOCON_MEM:
      if(pipe(ofd)==-1) return status;
      _rd_worker(&status->out, out, ofd);
      close(ofd[0]);
      //buprintv(1, "ofd[1] == %%\n", d(ofd[1]));
      break;
    }

  switch(err->type)
    {
    case IOCON_CTRLFD:
      if(pipe(efd)==-1) return status;
      err->ep.ctrlfd = efd[0];
      break;
    case IOCON_MEM:
      if(pipe(efd)==-1) return status;
      _rd_worker(&status->err, err, efd);
      close(efd[0]);
      break;
    }

  switch(in->type)
    {
    case IOCON_MEM:
      if(pipe(ifd)==-1) return status;
      break;
    case IOCON_CTRLFD:
      if(pipe(ifd)==-1) return status;
      in->ep.ctrlfd = ifd[1];
      break;
    }

  pid = fork();
  if(pid == -1) return status;
  
  if(pid == 0)
    {
      _ipc_child(in, out, err, ifd, ofd, efd);
      execve(fn, argv, envp);
      while(kill(getpid(), 9)==0)
	sleep(1);
      _exit(EX_UNAVAILABLE); /* error */
    }
  if(argv && (opt_argv == NULL)) free((void*) argv);
  
  switch(in->type)
    {
    case IOCON_CTRLFD:
      close(ifd[0]);
      break;
    case IOCON_PIPEFD:
      close(in->ep.pipefd);
      break;
    case IOCON_MEM:
      close(ifd[0]);
      close(ifd[1]);
      break;
    }
  
  switch(out->type)
    {
    case IOCON_CTRLFD:
      close(ofd[1]);
      break;
    case IOCON_PIPEFD:
      break;
    case IOCON_MEM:
      close(ofd[1]);
      break;
    }
  
  switch(err->type)
    {
    case IOCON_CTRLFD:
      close(efd[1]);
      break;
    case IOCON_PIPEFD:
      break;
    case IOCON_MEM:
      close(efd[1]);
      break;
    }
  
  status->pid = pid;
  
  return status;
}
