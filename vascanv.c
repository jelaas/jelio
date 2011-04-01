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

int vascanv(struct jelio_buf *jb, int *opt_consumed, int *eof, int *failed, const char *fmt,
	    va_list ap)
{
  int c, rc;
  int consumed;
  int count=0;
  
  if(!opt_consumed) opt_consumed = &consumed;
  *opt_consumed = 0;
  
  while(*fmt)
    {
      c = jelio_buffer_peekc(jb);
      if(c == -1) *eof=1;
      switch(*fmt)
	{
	case '%':
	  if(*(fmt+1) == '%')
	    {
	      struct jelio_input *ji;
	      fmt++;
	      ji = va_arg(ap, struct jelio_input*);
	      if(ji)
		{
		  rc = ji->callback(ji->data, ji->valsize, jb);
		  i_default_free(ji);
		  if(rc <= 0)
		    {
		      if(rc == -1) *failed = *eof=1;
		      goto scanout;
		    }
		  (*opt_consumed) += rc;
		  count++;
		}
	    }
	  else
	    {
	      if(*fmt != c)
		{
		  fmt++;
		  goto scanout;
		}
	      jelio_buffer_getc(jb);
	      (*opt_consumed)++;
	    }
	  break;
	case ' ':
	case '\t':
	case '\n':
	  /* eat whitespace. non or any amount */
	  /* EOF will count as whitespace */
	  if(c == -1) break;
	  while(char_isspace(c))
	    {
	      jelio_buffer_getc(jb);
	      (*opt_consumed)++;
	      c = jelio_buffer_peekc(jb);
	      if(c == -1) break;
	    }
	  break;
	case '\\':
	  if(*(fmt+1))
	    fmt++;
	default:
	  if(*fmt != c)
	    {
	      if(c == -1) *failed = 1;
	      goto scanout;
	    }
	  jelio_buffer_getc(jb);
	  (*opt_consumed)++;
	  break;
	}
      fmt++;
    }
  return count;

 scanout:
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
	  struct jelio_input *ji;
	  fmt++;
	  ji = va_arg(ap, struct jelio_input*);
	  if(ji) i_default_free(ji);
	}
      fmt++;
    }
  return count;
}
