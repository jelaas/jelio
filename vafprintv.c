/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include "jeliof.h"
#include "jelio_internal.h"

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
  %% -> arg expected
  \  -> escape next character
*/

int vafprintv(FILE *f, char *fmt, va_list ap)
{
  int wrote=0;
  
  while(*fmt)
    {
      switch(*fmt)
	{
	case '\\':
	  if(*(fmt+1))
	    {
	      fmt++;
	      if(fputc(*fmt, f)==EOF)
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
		    jelio_prot(s->mark);
		    if(fputs(s->data, f)==EOF)
		      goto scanout;
		    wrote+=strlen(s->data);
		    free(s);
		  }
		fmt++;
	      }
	    else
	      {
		if(fputc(*fmt, f) == EOF)
		  goto scanout;
		wrote++;
	      }
	  break;
	default:
	  if(fputc(*fmt, f)==EOF)
	    goto scanout;
	  wrote++;
	  break;
	}
      fmt++;
    }
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
 return EOF;
}
