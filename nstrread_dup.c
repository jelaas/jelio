/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens L��s, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <unistd.h>
#include <stdlib.h>

#include "jelio.h"
#include "jelio_internal.h"

size_t nstrread_dup(int fd, char **s)
{
  char buf[8], *p;
  size_t len=0;
  
  if(jelio_read(fd, buf, 4, NULL)!=4) return -1;
  if(buf[3] != ':')
    {
      if(jelio_read(fd, buf+4, 4, NULL)!=4) return -1;
      if(buf[7]!=':') return -1;
      buf[7] = 0;
    }
  else
    buf[3] = 0;
  for(p=buf;*p;p++)
    {
      len *= 10; 
      len += *p -'0';
    }
  *s = malloc(len+2);
  if(!*s) return -1;
  
  if(jelio_read(fd, *s, len+1, NULL)!=len+1)
    {
      free(*s);
      *s = NULL;
      return -1;
    }
  if(*(*s+len) != ',')
    {
      free(*s);
      *s = NULL;
      return -1;
    }
  *(*s+len) = 0;
  return len;
}
