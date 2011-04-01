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

int cprintv(const char *fmt, ...)
{
  int n=0;
  va_list ap;
  va_start(ap, fmt);

  while(*fmt)
    {
      switch(*fmt)
	{
	case '\\':
	  if(*(fmt+1))
	    {
	      fmt++;
	      n++;
	    }
	  break;
	case '%':
	    if(*(fmt+1) == '%')
	      {
		jelio *s;
		s = va_arg(ap, jelio*);
		if(s)
		  {
		    jelio_prot(s->mark);
		    n+=strlen(s->data);
		    free(s);
		  }
		fmt++;
	      }
	    else
	      n++;
	  break;
	default:
	  n++;
	  break;
	}
      fmt++;
    }
  va_end(ap);
  return n;
}
