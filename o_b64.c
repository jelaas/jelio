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

static size_t base64ntostr(char *dst, const char *src, size_t srclen)
{
  const unsigned char *s = (unsigned char*) src;
  int v, i, phase;
  unsigned char *p = (unsigned char*) dst;
  v = i = phase = 0;
  
  *p = 0;
  while(*s && srclen--)
    {
      unsigned int c = *s++;
      i = -1;
      if(c >= 'A' && c <= 'Z')
        i = c - 'A';
      if(c >= 'a' && c <= 'z')
        i = c - 'a' + 26;
      if(c >= '0' && c <= '9')
        i = c - '0' + 52;
      if(c == '+')
        i = 62;
      if(c == '/')
        i = 63;
      if(i >= 0)
        {
          switch(phase)
            {
            case 0:
              v = i;
              break;
            case 1:
              *p++ = (v << 2) + ((i & 0x30) >> 4);
              v = i << 4;
              break;
            case 2:
              *p++ = v + (i >> 2);
              v = i & 3;
              break;
            case 3:
              *p++ = (v << 6) + i;
              phase = -1;
              break;
            }
          phase++;
        }
      *p = 0;
    }
  return p-(unsigned char *)dst;
}

jelio *o_b64(const char *base64, size_t srclen)
{
  char *buf, *p;
  if(!base64) return NULL;
  if(!srclen) srclen = strlen(base64);
  buf = malloc((srclen+1)+JELIOSIZE+1);
  if(!buf) return NULL;
  jelio_protcpy(buf, buf+JELIOSIZE);
  
  p = buf+JELIOSIZE;
  
  base64ntostr(p, base64, srclen);
  
  return (jelio*) buf;
}
