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

static int i_s_conv(void *value, int n, void *inbuf)
{
  struct jelio_buf *jb = inbuf;
  char *sval = value;
  int c,count=0, wcount=0;
  
  /* scan past whitespace */
  for(c = jelio_buffer_peekc(jb);char_isspace(c);c = jelio_buffer_peekc(jb))
    {
      jelio_buffer_getc(jb);
      wcount++;
    }
  if(n < 1) return 0;
  n--;
  
  while(1)
    {
      if(count >= n)
	break;
      c = jelio_buffer_peekc(jb);
      if(c == -1 || char_isspace(c))
	break;
      count++;
      jelio_buffer_getc(jb);
      if(sval) *sval++ = c;
    }
  if(sval) *sval = 0;
  if(!count)
    {
      if(c == -1) return -1;
      return 0;
    }
  return count+wcount;
}

struct jelio_input *i_s(char *opt_s, int valsize)
{
  struct jelio_input *ji;
  ji = malloc(sizeof(struct jelio_input));
  
  ji->data = opt_s;
  ji->valsize = valsize;
  ji->callback = i_s_conv;
  ji->free = NULL;
  return ji;
}

