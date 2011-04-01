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

static int i_sdup_conv(void *value, int n, void *inbuf)
{
  struct jelio_buf *jb = inbuf;
  char **sval = value;
  int c,count=0, wcount=0;
  size_t ulen, mlen;
  
  *sval = NULL;
  
  /* scan past whitespace */
  for(c = jelio_buffer_peekc(jb);char_isspace(c);c = jelio_buffer_peekc(jb))
    {
      jelio_buffer_getc(jb);
      wcount++;
    }
  
  while(1)
    {
      c = jelio_buffer_peekc(jb);
      if(c == -1 || char_isspace(c))
	break;
      count++;
      jelio_buffer_getc(jb);
      sbuprintv(sval, &ulen, &mlen, 1024*1024, "%%", o_c(c));
    }
  if(!count)
    {
      if(c == -1) return -1;
      return 0;
    }
  return count+wcount;
}

struct jelio_input *i_sdup(char **s)
{
  struct jelio_input *ji;
  ji = malloc(sizeof(struct jelio_input));
  
  ji->data = s;
  ji->valsize = 0;
  ji->callback = i_sdup_conv;
  ji->free = NULL;
  return ji;
}

