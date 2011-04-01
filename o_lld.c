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
#include <stdlib.h>
#include <string.h>

jelio *o_lld(long long int i)
{
  char *buf;
  int l;
  /*                digits           sign 0 NULL */
  l = (((sizeof(long long int)*8) / 6)*2) +1 +1 +1;
  buf = malloc(l+JELIOSIZE);
  jelio_protcpy(buf, buf+JELIOSIZE);
  jelio_llitostr(buf+JELIOSIZE, l, i, 10);
  return (jelio*) buf;
}
