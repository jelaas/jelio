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

#include <string.h>
#include <stdlib.h>

jelio *o_nstr(const char *s)
{
  char *buf;
  size_t n, len, nlen;
  
  if(!s) return NULL;
  len = strlen(s);
  if(len > 999)
    nlen = 8;
  else
    nlen = 4;
  
  buf = malloc(len+1+nlen+JELIOSIZE+1);
  if(!buf) return NULL;

  jelio_protcpy(buf, buf+JELIOSIZE);
  
  n = len;
  if(nlen == 4)
    {
      *(buf+JELIOSIZE+3) = ':';
      *(buf+JELIOSIZE+2) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+1) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+0) = (n % 10) +'0'; n = n/10;
    }
  else
    {
      *(buf+JELIOSIZE+7) = ':';
      *(buf+JELIOSIZE+6) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+5) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+4) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+3) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+2) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+1) = (n % 10) +'0'; n = n/10;
      *(buf+JELIOSIZE+0) = (n % 10) +'0'; n = n/10;
    }

  memcpy(buf+JELIOSIZE+nlen, s, len);
  *(buf+JELIOSIZE+len+nlen)=',';
  *(buf+JELIOSIZE+len+nlen+1)=0;
  return (jelio*) buf;
}
