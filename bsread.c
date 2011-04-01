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
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

static unsigned int readbits(struct jelbs *s, int n, int *eof)
{
  unsigned int o=0, t;

  while(n)
    {
      if(bseof(s))
	{
	  *eof=1;
	  return o;
	}
      if(n > (s->bitpos))
	{
	  /* read all there is */

	  o <<= (s->bitpos+1);
	  t = *(s->mem + s->bytepos);
	  t &= (1 << (s->bitpos+1))-1;
	  o |= t;
	  
	  n -= (s->bitpos+1);
	  s->bitpos = 7;
	  s->bytepos++;
	}
      else
	{
	  /* read part of what there is */
	  o <<= n;
	  t = *(s->mem + s->bytepos);
	  t >>= (s->bitpos+1) - n;
	  t &= (1 << n)-1;
	  o |= t;
	  s->bitpos -= n;
	  if(s->bitpos < 0)
	    {
	      s->bitpos = 7;
	      s->bytepos++;
	    }
	  break;
	}
    }
  return o;
}

static int readbytes(struct jelbs *s, unsigned char *oct, int n)
{
  int eof=0;
  while(n--)
    {
      if(bseof(s))
	return -1;
      *oct++ = readbits(s, 8, &eof);
      if(eof) return -1;
    }
  return 0;
}

int bsread(struct jelbs *s, const char *fmt, ...)
{
  int count=0, downshift;
  int opt, len, consumed;
  unsigned int *v;
  unsigned char *str;
  unsigned char *oct;
  va_list ap;
  int eof=0;

  if(s->eof)
    return -1;

  va_start(ap, fmt);
  while(*fmt)
    {
      downshift=0;
      opt = *fmt++;
      switch(opt)
	{
	case 's':
	  str = va_arg(ap, unsigned char*);
	  if(sscanv(fmt, &consumed, "%%", i_d(&len))==1)
	    {
	      fmt+=consumed;
	      
	      /* read 'len' characters from stream into 'str' */
	      if(readbytes(s, str, len))
		return -1;
	      *(str+len) = 0;
	      count++;
	    }
	  break;
	case 'o':
	  oct = va_arg(ap, unsigned char*);
	  if(sscanv(fmt, &consumed, "%%", i_d(&len))==1)
	    {
	      fmt+=consumed;
	      
	      /* read 'len' octets from stream into 'oct' */
	      if(readbytes(s, oct, len))
		return -1;
	      count++;
	    }
	  break;
	case 'B':
	  downshift=1;
	case 'b':
	  v = va_arg(ap, unsigned int*);
	  if(sscanv(fmt, &consumed, "%%", i_d(&len))==1)
	    {
	      fmt+=consumed;
	      *v = readbits(s, len, &eof);
	      if(eof) return -1;
	      if(!downshift)
		(*v) <<= 32-len;
	      count++;
	    }
	  break;
	default:
	  break;
	}
    }

  va_end(ap);
  return count;
}
