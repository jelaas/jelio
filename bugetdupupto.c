/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <stdlib.h>
#include <string.h>

#include "jelio.h"
#include "jelio_internal.h"

/*
  Upto "upto" EOF maxlen.
  NULL term.
  "upto" stored in buffer if encountered.
   Overflow is lost.
   eof marked if EOF encountered.
   -1 on error.
   upto_len == 0, upto_len = strlen(upto)
*/

int bugetdupupto(int fd, int *opt_eof, int maxlen, int *opt_lost, char **dbuf, char *upto, int upto_len)
{
  int count=0;
  int buf;
  struct jelio_buf *jb;
  size_t mlen=0;
  char *tbuf = NULL, *rbuf = *dbuf;
  
  if(opt_eof) *opt_eof = 0;
  if(opt_lost) *opt_lost = 0;
  if(maxlen<2)
    {
      if(*dbuf) **dbuf = 0;
      return 0;
    }
  if(!rbuf) 
    {
      /* create a dynamic buffer */
      mlen = 32;
      rbuf = malloc(mlen+1);
      if(!rbuf)
	return -1;
    }
  maxlen--;

  if(upto_len == 0) upto_len = strlen(upto);
  
  jb = jelio_buffer(fd);
  if(!jb) return -1;

  while(1)
    {
      buf = jelio_buffer_getc(jb);
      if(buf == -1)
	{
	  if(opt_eof) *opt_eof = 1;
	  break;
	}
      if(count < maxlen)
	{
	  if(mlen && count >= mlen)
	    {
	      /* this is a dynamic buffer which needs to be increased */
	      mlen = mlen << 1;
	      rbuf = realloc(rbuf, mlen+1);
	      if(!rbuf) return -1;
	    }
	  *(rbuf+count) = buf;
	  count++;
	  if(count >= upto_len)
	    if(!memcmp(rbuf+count-upto_len, upto, upto_len))
	      break;
	}
      else
	{
	  if(!tbuf)
	    {
	      /* load tail buffer */
	      tbuf = malloc(upto_len);
	      if(!tbuf) return -1;
	      if(count >= upto_len)
		memcpy(tbuf, rbuf+count-upto_len, upto_len);
	      else
		memcpy(tbuf+upto_len-count, rbuf, count);
	    }
	  if(opt_lost) (*opt_lost)++;
	  memmove(tbuf, tbuf+1, upto_len-1);
	  *(tbuf+upto_len-1) = buf;
	  if(!memcmp(tbuf, upto, upto_len))
	    {
	      
	      break;
	    }
	}
      
    }
  if(tbuf) free(tbuf);
  if(mlen) *dbuf = rbuf;
  *(rbuf+count) = 0;
  return count;
}
