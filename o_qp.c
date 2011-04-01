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

static const char *HEXDIGITS="0123456789ABCDEF";
static size_t qpntostr(char *dst,
		       const char *src,
		       size_t srclen)
{
  const unsigned char *s = (unsigned char*) src;
  unsigned char *p;
  char *x1, *x2;
  p = (unsigned char*) dst;
  
  while(*s && srclen--)
    {
      unsigned int c = *s++;
      if( (c == '=') && (srclen > 1))
        {
          if((x1 = strchr(HEXDIGITS, *s)) &&
             (x2 = strchr(HEXDIGITS, *(s+1))) )
            {
              c = (x1-HEXDIGITS) << 4;
              c += (x2-HEXDIGITS);
              *p++ = c;
              s += 2;
              srclen -= 2;
              continue;
            }
          if( (*s == '\r') &&
              (*(s+1) == '\n') )
            {
              s += 2;
              srclen -= 2;
              continue;
            }
        }
      if( (c == '\r') && (*s == '\n') && (srclen>0))
        {
          *p++ = '\n';
          srclen--;
          s++;
          continue;
        }
      *p++ = c;
    }
  *p = 0;
  return p-(unsigned char *)dst;
}

jelio *o_qp(const char *qpstr, size_t srclen)
{
  char *buf, *p;
  if(!qpstr) return NULL;
  if(!srclen) srclen = strlen(qpstr);
  buf = malloc((srclen+1)+JELIOSIZE+1);
  if(!buf) return NULL;
  jelio_protcpy(buf, buf+JELIOSIZE);
  
  p = buf+JELIOSIZE;
  
  qpntostr(p, qpstr, srclen);
  
  return (jelio*) buf;
}
