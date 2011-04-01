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

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*
  %% -> arg expected
  \  -> escape next character
*/

int sprintv(char *buf, size_t maxlen, const char *fmt, ...)
{
  int n=0;
  va_list ap;
  maxlen--;
  
  va_start(ap, fmt);
  if(maxlen < 1)
    {
      if(maxlen == 0)
	*buf = 0;
      goto scanout;
    }
  
  while(*fmt)
    {
      switch(*fmt)
	{
	case '\\':
	  if(*(fmt+1))
	    {
	      fmt++;
	      buf[n++] = *fmt;
	    }
	  break;
	case '%':
	    if(*(fmt+1) == '%')
	      {
		jelio *s;
		s = va_arg(ap, jelio*);
		if(s)
		  {
		    int len;
		    jelio_prot(s->mark);
		    len=strlen(s->data);
		    if(len)
		      {
			if(len + n >= maxlen )
			  {
			    strncpy(buf+n, s->data, maxlen-n);
			    n += (maxlen-n);
			    buf[n] = 0;
			    free(s);
			    goto scanout;
			  }
			strcpy(buf+n, s->data);
			n+=len;
		      }
		    free(s);
		  }
		fmt++;
	      }
	    else
	      buf[n++] = *fmt;
	  break;
	default:
	  buf[n++] = *fmt;
	  break;
	}
      if(n >= maxlen)
	{
	  buf[n]=0;
	  goto scanout;
	}
      fmt++;
    }
  buf[n]=0;
  va_end(ap);
  return n;
 scanout:
  /* need to free all string args */
  fmt++;
  while(*fmt)
    {
      if( (*fmt == '\\') &&
	  *(fmt+1) )
	{
	  fmt+=2;
	  continue;
	}
      if( (*fmt == '%') && 
	  (*(fmt+1) == '%') )
	{
	  char *s;
	  s = va_arg(ap, char*);
	  if(s) free(s);
	  fmt++;
	}
      fmt++;
    }
  va_end(ap);
  return n;
}
