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

ssize_t jelio_write(int fd, const void *buf, size_t count)
{
  ssize_t sofar=0;
  ssize_t wrote;
  
  while(sofar < count)
    {
      wrote = write(fd, buf+sofar, count-sofar);
      if(wrote <= 0)
	{
	  if( (wrote<0) && (errno == EINTR)) continue;
	  return wrote;
	}
      sofar += wrote;
    }
  return sofar;
}
