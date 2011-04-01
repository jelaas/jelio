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
off_t jelio_buffer_seek(struct jelio_buf *jbuf, off_t offset, int whence)
{
  off_t n;
  
  if( (whence == SEEK_CUR) && (offset == 0) )
    {
      n = lseek(jbuf->fd, 0, SEEK_CUR);
      return n-jbuf->len+jbuf->pos;
    }
  
  if(whence == SEEK_CUR)
    {
      if( (jbuf->pos + offset >= jbuf->len) ||
	  (jbuf->pos + offset < 0) )
	{
	  /* moving outside buffer */
	  jbuf->pos = jbuf->len = 0;
	  return lseek(jbuf->fd, offset, SEEK_CUR);
	}
      
      /* movement inside the buffer */
      jbuf->pos += offset;
      n = lseek(jbuf->fd, 0, SEEK_CUR);
      return n-jbuf->len+jbuf->pos;      
    }

  if(whence == SEEK_SET)
    {
      /* current location for start of buffer */
      n = lseek(jbuf->fd, 0, SEEK_CUR);
      n = n-jbuf->len;
      
      if( (offset >= n+jbuf->len) ||
	  (offset < n) )
	{
	  /* moving outside buffer */
	  jbuf->pos = jbuf->len = 0;
	  return lseek(jbuf->fd, offset, SEEK_SET);	  
	}
      jbuf->pos = offset - n;
      return offset;
    }
  
  if(whence == SEEK_END)
    {
      /* always clear buffer and do seek.
	 FIXME: should we do stat() and work from the filesize ?
       */
      jbuf->pos = jbuf->len = 0;
      return lseek(jbuf->fd, offset, SEEK_END);	  
    }
  
  errno = EINVAL;
  return (off_t) -1;
}
