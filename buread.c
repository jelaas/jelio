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

int buread(int fd, int *opt_eof, int len, void *opt_buf)
{
  int count=0;
  int buf;
  struct jelio_buf *jb;

  if(opt_eof) *opt_eof = 0;

  jb = jelio_buffer(fd);
  while(len--)
    {
      buf = jelio_buffer_getc(jb);
      if(buf == -1)
	{
	  if(opt_eof) *opt_eof = 1;
	  break;
	}
      if(opt_buf)
	*((unsigned char *)opt_buf+count) = buf;
      count++;
    }
  return count;
}
