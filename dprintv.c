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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*
  %% -> arg expected
  \  -> escape next character
*/

int dprintv(int fd, const char *fmt, ... )
{
  va_list ap;
  int wrote=0;
  
  va_start(ap, fmt);
  while(*fmt)
    {
      switch(*fmt)
	{
	case '\\':
	  if(*(fmt+1))
	    {
	      fmt++;
	      if(jelio_write(fd, fmt, 1)!=1)
		goto scanout;
	      wrote++;
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
		    len = strlen(s->data);
		    if(len)
		      {
			if(jelio_write(fd, s->data, len)!=len)
			  {
			    free(s);
			    goto scanout;
			  }
			wrote+=len;
		      }
		    free(s);
		  }
		fmt++;
	      }
	    else
	      {
		if(jelio_write(fd, fmt, 1)!=1)
		  goto scanout;
		wrote++;
	      }
	  break;
	default:
	  if(jelio_write(fd, fmt, 1)!=1)
	    goto scanout;
	  wrote++;
	  break;
	}
      fmt++;
    }
  va_end(ap);
  return wrote;

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
  return -1;
}
