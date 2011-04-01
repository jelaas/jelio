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

int vasbuprintv(struct xcontext *c, const char *fmt, va_list ap)
{
  int len;
  int n=0;
  char *tmp;

  if(*c->dbuf == NULL)
    {
      c->mlen = strlen(fmt)+1;
      c->ulen = 0;
      *c->dbuf = malloc(c->mlen);
      if(!*c->dbuf)
	{
	  n = -1;
	  goto scanout;
	}
      **c->dbuf = 0;
    }
  
  if(c->maxlen < 1)
    {
      if(c->maxlen == 0)
	**c->dbuf = 0;
      n = -2;
      goto scanout;
    }

  if(c->ulen >= c->maxlen)
    {
      /* maxlen reached */
      n = -2;
      goto scanout;
    }

  if(c->prefix)
    {
      /* add prefix to buffer */
      len = strlen(c->prefix);
      if(len)
	{
	  if(len + c->ulen >= c->mlen )
	    {
	      c->mlen = len + c->ulen +1;
	      if(c->mlen >= c->maxlen)
		c->mlen = c->maxlen+1;
	      tmp = realloc(*c->dbuf, c->mlen);
	      if(!tmp)
		{
		  n = -1;
		  goto scanout;
		}
	      *c->dbuf = tmp;
	    }
	  if(len + c->ulen >= c->maxlen )
	    {
	      strncpy((*c->dbuf)+c->ulen, c->prefix,
		      c->maxlen-c->ulen);
	      n += (c->maxlen-c->ulen);
	      c->ulen += (c->maxlen-c->ulen);
	      (*c->dbuf)[c->ulen] = 0;
	      goto scanout;
	    }
	  strcpy((*c->dbuf)+c->ulen, c->prefix);
	  n+=len;
	  (c->ulen) += len;
	}
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
	      (*c->dbuf)[(c->ulen)++] = *fmt;
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
		    len=strlen(s->data);
		    if(len)
		      {
			if(len + c->ulen >= c->mlen )
			  {
			    c->mlen = len + c->ulen +1;
			    if(c->mlen >= c->maxlen)
			      c->mlen = c->maxlen+1;
			    tmp = realloc(*c->dbuf, c->mlen);
			    if(!tmp)
			      {
				free(s);
				n = -1;
				goto scanout;
			      }
			    *c->dbuf = tmp;
			  }
			if(len + c->ulen >= c->maxlen )
			  {
			    strncpy((*c->dbuf)+c->ulen, s->data,
				    c->maxlen-c->ulen);
			    n += (c->maxlen-c->ulen);
			    c->ulen += (c->maxlen-c->ulen);
			    (*c->dbuf)[c->ulen] = 0;
			    free(s);
			    goto scanout;
			  }
			strcpy((*c->dbuf)+c->ulen, s->data);
			n+=len;
			(c->ulen) += len;
		      }
		    free(s);
		  }
		fmt++;
	      }
	    else
	      {
		n++;
		(*c->dbuf)[(c->ulen)++] = *fmt;
	      }
	  break;
	default:
	  (*c->dbuf)[(c->ulen)++] = *fmt;
	  n++;
	  break;
	}
      if(c->ulen==c->mlen)
	{
	  c->mlen += 8;
	  *c->dbuf = realloc(*c->dbuf, c->mlen);
	  if(!(*c->dbuf)) goto scanout;
	}
      if(c->ulen >= c->maxlen)
	{
	  (*c->dbuf)[c->ulen]=0;
	  goto scanout;
	}
      fmt++;
    }
  (*c->dbuf)[c->ulen]=0;
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
  return n;
}
