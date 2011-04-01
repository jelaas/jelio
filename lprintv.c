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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
  %% -> arg expected
  \  -> escape next character
*/

#define BL 255
int lprintv(char *fn, mode_t mode, const char *fmt, ... )
{
  int fd;
  char buf[BL+1];
  int n=0;
  int wrote=0;
  va_list ap;
  
  fd = open(fn, O_RDWR|O_APPEND|O_CREAT, mode);
  if(fd < 0) return -1;
  
  va_start(ap, fmt);
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
			if( (len >= BL) || (len + n >= BL) )
			  {
			    if(jelio_write(fd, buf, n)!=n)
			      {
				free(s);
				goto scanout;
			      }
			    wrote += n;
			    n=0;
			  }
			if(len >= BL)
			  {
			    if(jelio_write(fd, s->data, len)!=len)
			      {
				free(s);
				goto scanout;
			      }
			    wrote += len;
			  }
			else
			  {
			    strcpy(buf+n, s->data);
			    n+=len;
			  }
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
      if(n >= BL)
	{
	  if(jelio_write(fd, buf, n)!=n)
	    goto scanout;
	  wrote += n;
	  n=0;
	}
      fmt++;
    }
  if(n)
    {
      if(jelio_write(fd, buf, n)!=n)
	{
	  close(fd);
	  return -1;
	}
      wrote += n;
    }
  close(fd);
  va_end(ap);
  return wrote;

 scanout:
  /* need to free all string args */
  close(fd);
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
