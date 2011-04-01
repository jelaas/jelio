/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <unistd.h>
#include <errno.h>
#include "jelio.h"

ssize_t jelio_read(int fd, void *buf, size_t count, int *opt_err)
{
  int got,sofar=0;
  
  if( opt_err) *opt_err = 0;
  
  while(sofar<count)
    {
      got = read(fd, buf+sofar, count-sofar);
      if(got <= 0)
	{
	  if( (got<0) && (errno == EINTR)) continue;
	  if( (got<0) && opt_err) *opt_err = -1;
	  break;
	}
      sofar += got;
    }
  return sofar;
}
