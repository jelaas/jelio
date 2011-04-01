/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2006
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include "jelio.h"
#include "jelio_internal.h"

#include <string.h>
#include <stdlib.h>

static size_t urlntostr(char *dst, const char *src, size_t srclen)
{
  /* %XX -> Char */
  char *s = (char *)src;
  unsigned int x1,x2;
  unsigned char *p;
  p = (unsigned char *)dst;
  
  while(*s && srclen--)
    {
      unsigned int c = *s++;
      if(c == '%' && (srclen > 1) && *s)
        {
          x1 = *(s);
          x2 = *(s+1);

	  if( (x1 >= 'A' && x1 <= 'F') ||
	      (x1 >= 'a' && x1 <= 'f') ||
	      (x1 >= '0' && x1 <= '9') )
	    {
	      if( (x2 >= 'A' && x2 <= 'F') ||
		  (x2 >= 'a' && x2 <= 'f') ||
		  (x2 >= '0' && x2 <= '9') )
		{
		  if(x1 >= 'A' && x1 <= 'F')
		    *p = (x1 - 'A' + 10) << 4;
		  if(x1 >= 'a' && x1 <= 'f')
		    *p = (x1 - 'a' + 10) << 4;
		  if(x1 >= '0' && x1 <= '9')
		    *p = (x1 - '0') << 4;

		  if(x2 >= 'A' && x2 <= 'F')
		    *p += (x2 - 'A' + 10);
		  if(x2 >= 'a' && x2 <= 'f')
		    *p += (x2 - 'a' + 10);
		  if(x2 >= '0' && x2 <= '9')
		    *p += (x2 - '0');

		  srclen-=2;
		  s+=2;
		  p++;
		  continue;
		}
	    }
	}
      *p++ = c;
    }
  *p = 0;
  return p-(unsigned char *)dst;
}


jelio *o_url(const char *urlstr, size_t srclen)
{
  char *buf, *p;
  if(!urlstr) return NULL;
  if(!srclen) srclen = strlen(urlstr);
  buf = malloc((srclen+1)+JELIOSIZE+1);
  if(!buf) return NULL;
  jelio_protcpy(buf, buf+JELIOSIZE);
  
  p = buf+JELIOSIZE;
  
  urlntostr(p, urlstr, srclen);
  
  return (jelio*) buf;
}
