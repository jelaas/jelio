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
#include <stdlib.h>

/* cleanup! Dont forget or strange things may happen! */
int buclose(int fd)
{
  struct jelio_buf **inputbuffers;
  struct jelio_buf *buf, *pbuf=0;

  inputbuffers = jelio_inputbuffers();
  
  for(buf=*inputbuffers;buf;buf=buf->next)
    {
      if(buf->fd == fd)
	break;
      pbuf=buf;
    }
  if(buf)
    {
      if(pbuf)
	pbuf->next = buf->next;
      else
	*inputbuffers = buf->next;
      if(buf->buf) free(buf->buf);
      free(buf);
      return 0;
    }
  return -1;
}


