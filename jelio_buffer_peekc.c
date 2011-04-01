/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include "jelio.h"
#include <errno.h>
#include "jelio_internal.h"

 /* char or EOF */
int jelio_buffer_peekc(struct jelio_buf *jbuf)
{
  /* buffer empty? */
  if(jbuf->pos >= jbuf->len)
    {
      int got;
      if(jbuf->eof) return -1;
      
      /* filling up! */
    retry:
      got = read(jbuf->fd, jbuf->buf, JELIO_IBUFSIZE);

      if(got < 1)
	{
	  if( (got<0) && (errno == EINTR)) goto retry;
	  jbuf->eof = 1;
	  return -1;
	}
      
      jbuf->len = got;
      jbuf->pos = 0;
    }

  /* get next */
  return jbuf->buf[jbuf->pos];
}
