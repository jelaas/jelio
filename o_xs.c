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

static char hexdigits[16] = "0123456789abcdef";
jelio *o_xs(const unsigned char *s, int n)
{
  char *buf, *p;
  if(!s) return NULL;
  buf = malloc((n*2)+JELIOSIZE+1);
  if(!buf) return NULL;
  jelio_protcpy(buf, buf+JELIOSIZE);

  p = buf+JELIOSIZE;
  while(n--)
    {
      *p++ = hexdigits[((*s) >> 4)];
      *p++ = hexdigits[((*s++) & 15)];
    }
  *p = 0;
  return (jelio*) buf;
}
