/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include "jelio.h"
#include "jelio_internal.h"


/*
FIXME: How to handle readupto ? Wait for more data or return partial
            upto one byte at a time ? Maybe timeout ? ...
       Maybe there is no way to fix it since we dont know why the user polls.
*/

int bupoll(struct pollfd *ufds, unsigned int nfds, int timeout)
{
  struct jelio_buf **inputbuffers;
  struct jelio_buf *buf;
  int i, n=0;

  inputbuffers = jelio_inputbuffers();
  
  /* first check on data for fd's that are buffered by jelio */
  for(buf=*inputbuffers;buf;buf=buf->next)
    {
      for(i=0;i<nfds;i++)
	if((buf->fd == ufds[i].fd) && (ufds[i].events & POLLIN))
	{
	  if(buf->pos < buf->len)
	    {
	      /* data left to read */
	      ufds[i].revents |= POLLIN;
	      n++;
	    }
	}
    }
  if(n) return n;
  
  /* do regular poll */
  return poll(ufds, nfds, timeout);
}
