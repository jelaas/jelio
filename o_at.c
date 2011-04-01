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
#include <time.h>

jelio *o_at(time_t i)
{
  char *buf;
  int l;
  struct tm tm;

  l = 4+3+3+1+3+3+3;
  buf = malloc(l+JELIOSIZE);
  
  gmtime_r(&i, &tm);
  
  jelio_protcpy(buf, buf+JELIOSIZE);
  sprintv(buf+JELIOSIZE, l, "%%-%%-%% %%:%%:%%",
	  dF(tm.tm_year+1900, 4, 4), dF(tm.tm_mon+1, 2, 2), dF(tm.tm_mday, 2, 2),
	  dF(tm.tm_hour, 2, 2), dF(tm.tm_min, 2, 2), dF(tm.tm_sec, 2, 2));
  
  return (jelio*) buf;
}
