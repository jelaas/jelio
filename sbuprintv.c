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

int sbuprintv(char **buf, size_t *ulen, size_t *mlen, size_t maxlen,
	     const char *fmt, ...)
{
  int n=0;
  char *tmp;
  va_list ap;

  if(*buf == NULL)
    {
      *mlen = strlen(fmt)+1;
      *ulen = 0;
      *buf = malloc(*mlen);
      if(!*buf)
	{
	  n = -1;
	  va_start(ap, fmt);
	  goto scanout;
	}
      **buf = 0;
    }
  
  maxlen--;
  va_start(ap, fmt);
  
  if(maxlen < 1)
    {
      if(maxlen == 0)
	**buf = 0;
      n = -2;
      goto scanout;
    }

  if(*ulen >= maxlen)
    {
      /* maxlen reached */
      n = -2;
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
	      n++;
	      (*buf)[(*ulen)++] = *fmt;
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
			if(len + *ulen >= *mlen )
			  {
			    *mlen = len + *ulen +1;
			    if(*mlen >= maxlen)
			      *mlen = maxlen+1;
			    tmp = realloc(*buf, *mlen);
			    if(!tmp)
			      {
				free(s);
				n = -1;
				goto scanout;
			      }
			    *buf = tmp;
			  }
			if(len + *ulen >= maxlen )
			  {
			    strncpy((*buf)+*ulen, s->data, maxlen-*ulen);
			    n += (maxlen-*ulen);
			    *ulen += (maxlen-*ulen);
			    (*buf)[*ulen] = 0;
			    free(s);
			    goto scanout;
			  }
			strcpy((*buf)+*ulen, s->data);
			n+=len;
			(*ulen) += len;
		      }
		    free(s);
		  }
		fmt++;
	      }
	    else
	      {
		n++;
		(*buf)[(*ulen)++] = *fmt;
	      }
	  break;
	default:
	  (*buf)[(*ulen)++] = *fmt;
	  n++;
	  break;
	}
      if(*ulen==*mlen)
	{
	  *mlen += 8;
	  *buf = realloc(*buf, *mlen);
	  if(!(*buf)) goto scanout;
	}
      if(*ulen >= maxlen)
	{
	  (*buf)[*ulen]=0;
	  goto scanout;
	}
      fmt++;
    }
  (*buf)[*ulen]=0;
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
