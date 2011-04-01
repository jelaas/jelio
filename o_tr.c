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

jelio *o_tr(const const char *s, const char *from, const char *to)
{
  const char *p;
  char *b, *buf;
  int detect = 0;
  
  if(!s) return NULL;

  for(p=from;*p;p++)
    if(strchr(s, *p))
      {
	detect = 1;
	break;
      }
  if(!detect)
    return o_s(s);
  
  buf = malloc(strlen(s)+1+JELIOSIZE);
  if(!buf) return NULL;

  jelio_protcpy(buf, buf+JELIOSIZE);

  for(b=buf+JELIOSIZE;*s;s++)
    {
      if((p=strchr(from, *s)))
	{
	  if(to)
	    *b++ = *(to + (p-from));
	}
      else
	*b++ = *s;
    }
  *b = 0;
  return (jelio*) buf;
}
