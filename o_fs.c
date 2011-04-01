/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004-2006
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include "jelio.h"
#include "jelio_internal.h"

#include <string.h>
#include <stdlib.h>

jelio *o_fs(char *s)
{
  char *buf;

  buf = malloc((s?strlen(s):0)+JELIOSIZE+1);
  jelio_protcpy(buf, buf+JELIOSIZE);
  if(s)
    {
      strcpy(buf+JELIOSIZE, s);
      free(s);
    }
  else
    *(buf+JELIOSIZE)=0;
  
  return (jelio*) buf;
}
