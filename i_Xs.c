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

static int i_Xs_conv(void *value, int n, void *inbuf)
{
  struct jelio_buf *jb = inbuf;
  unsigned char *sval = value;
  unsigned int c;
  int count=0;
  int scount=0;
  int flag = 0;
  
  /* scan past whitespace */
  for(c = jelio_buffer_peekc(jb);char_isspace(c);c = jelio_buffer_peekc(jb))
    {
      jelio_buffer_getc(jb);
      count++;
    }
  if(n < 1) return 0;
  
  while(1)
    {
      if(scount >= n)
	break;
      c = jelio_buffer_peekc(jb);
      if(c == -1 || char_isspace(c))
	break;
      if( (c < '0' || c > '9') && (c < 'a' || c > 'f') && (c < 'A' || c > 'F'))
	break;
      count++;
      if(flag)
	{
	  if(c >= '0' && c <= '9')
	    *sval++ += (c - '0');
	  else
	    {
	      if(c >= 'a' && c <= 'f')
		*sval++ += (c - 'a' + 10);
	      else
		*sval++ += (c - 'A' + 10);
	    }
	  scount++;
	  flag = 0;
	}
      else
	{
	  if(c >= '0' && c <= '9')
	    *sval = (c - '0') << 4;
	  else
	    {
	      if(c >= 'a' && c <= 'f')
		*sval = (c - 'a' + 10) << 4;
	      else
		*sval = (c - 'A' + 10) << 4;
	    }
	  flag = 1;
	}
      jelio_buffer_getc(jb);
    }
  return count;
}

struct jelio_input *i_Xs(unsigned char *s, int valsize)
{
  struct jelio_input *ji;
  ji = malloc(sizeof(struct jelio_input));
  
  ji->data = s;
  ji->valsize = valsize;
  ji->callback = i_Xs_conv;
  ji->free = NULL;
  return ji;
}

