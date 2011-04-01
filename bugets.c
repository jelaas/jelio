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

int bugets(int fd, int *opt_eof, int maxlen, char *opt_buf)
{
  int count=0;
  int buf;
  struct jelio_buf *jb;
  
  if(opt_eof) *opt_eof = 0;
  if(maxlen<2)
    {
      if(opt_buf) *opt_buf = 0;
      return 0;
    }
  maxlen--;

  jb = jelio_buffer(fd);
  
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
	  if(opt_buf)
	    *(opt_buf+count) = buf;
	  count++;
	}
      if(buf == '\n')
	break;
    }
  if(opt_buf) *(opt_buf+count) = 0;
  return count;
}
