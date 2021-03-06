/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens L��s, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include "jelio.h"
#include "jelio_internal.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

jelio *o_t(time_t i)
{
  char *buf;
  int l;
  /*                digits           sign 0 NULL */
  l = (((sizeof(time_t)*8) / 6)*2) +1 +1 +1;
  buf = malloc(l+JELIOSIZE);
  jelio_protcpy(buf, buf+JELIOSIZE);
  if(sizeof(time_t) == sizeof(unsigned long long int))
    {
      jelio_llutostr(buf+JELIOSIZE, l, i, 10);
      return (jelio*) buf;
    }
  if(sizeof(time_t) == sizeof(unsigned long int))
    {
      jelio_lutostr(buf+JELIOSIZE, l, i, 10);
      return (jelio*) buf;
    }
  
  jelio_utostr(buf+JELIOSIZE, l, i, 10);
  return (jelio*) buf;
}
