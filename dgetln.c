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

int dgetln(int fd, int *opt_eof, int maxlen, char *opt_buf)
{
  int got,count=0;
  char buf;
  
  if(opt_eof) *opt_eof = 0;
  if(maxlen<2)
    {
      if(opt_buf) *opt_buf = 0;
      return 0;
    }
  maxlen--;
  while(1)
    {
      got = read(fd, &buf, 1);
      if(got <= 0)
	{
	  if( (got<0) && (errno == EINTR)) continue;
	  if(opt_eof) *opt_eof = 1;
	  break;
	}
      if(buf == '\n')
	break;
      if(buf == 0)
	break;
      if(count < maxlen)
	{
	  if(opt_buf)
	    *(opt_buf+count) = buf;
	  count++;
	}
    }
  if(opt_buf) *(opt_buf+count) = 0;
  return count;
}
