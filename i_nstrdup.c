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
#include <ctype.h>

static int i_nstrdup_conv(void *value, int n, void *inbuf)
{
  struct jelio_buf *jb = inbuf;
  char **sval = value;
  int i;
  char *p, buf[8];
  size_t len=0;

  buf[0] = jelio_buffer_getc(jb);
  buf[1] = jelio_buffer_getc(jb);
  buf[2] = jelio_buffer_getc(jb);
  buf[3] = jelio_buffer_getc(jb);
  if(buf[3] != ':')
    {
      buf[4] = jelio_buffer_getc(jb);
      buf[5] = jelio_buffer_getc(jb);
      buf[6] = jelio_buffer_getc(jb);
      buf[7] = jelio_buffer_getc(jb);
      if(buf[7] != ':')
	return -1;
      buf[7] = 0;
    }
  else
    buf[3] = 0;

  for(p=buf;*p;p++)
    {
      len *= 10; 
      len += *p -'0';
    }
  *sval = malloc(len+2);
  if(!*sval) return -1;
  
  for(i=0;i<=len;i++)
    *(*sval+i) = jelio_buffer_getc(jb);

  if(*(*sval+len) != ',')
    {
      free(*sval);
      *sval = NULL;
      return -1;
    }
  *(*sval+len) = 0;

  return len;
}

struct jelio_input *i_nstrdup(char **s)
{
  struct jelio_input *ji;
  ji = malloc(sizeof(struct jelio_input));
  ji->data = s;
  ji->valsize = 0;
  ji->callback = i_nstrdup_conv;
  ji->free = NULL;
  return ji;
}

