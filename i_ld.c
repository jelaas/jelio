/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens L��s, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include "jelio.h"
#include "jelio_internal.h"
#include <stdlib.h>
#include <ctype.h>

static int i_ld_conv(void *value, int n, void *inbuf)
{
  struct jelio_buf *jb = inbuf;
  long int *ival = value;
  int c,count=0,wcount=0;
  int mult = 1;

  *ival = 0;

  /* scan past whitespace */
  for(c = jelio_buffer_peekc(jb);char_isspace(c);c = jelio_buffer_peekc(jb))
    {
      jelio_buffer_getc(jb);
      wcount++;
    }

  c = jelio_buffer_peekc(jb);
  if(c == '-')
    {
      mult = -1;
      jelio_buffer_getc(jb);
      wcount++;
    }
  else
    {
      if(c == '+')
	{
	  jelio_buffer_getc(jb);
	  wcount++;
	}
    }
  
  while(1)
    {
      c = jelio_buffer_peekc(jb);
      if(c < '0' || c > '9')
	break;
      count++;
      jelio_buffer_getc(jb);
      *ival *= 10;
      *ival += c -'0';
    }
  if(!count) return 0;
  *ival *= mult;
  return count+wcount;
}

struct jelio_input *i_ld(long int *i)
{
  struct jelio_input *ji;
  ji = malloc(sizeof(struct jelio_input));
  
  ji->data = i;
  ji->callback = i_ld_conv;
  ji->free = NULL;
  return ji;
}

