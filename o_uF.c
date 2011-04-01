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

jelio *o_uF(unsigned int i, int spacefill, int zerofill)
{
  int l;
  char *buf;
  l = (((sizeof(int)*8) / 6)*2) +1 +1 +1;
  if(l < (spacefill+1))
    l = spacefill+1;
  if(l < (zerofill+1))
    l = zerofill+1;
  buf = malloc( l+JELIOSIZE );
  jelio_protcpy(buf, buf+JELIOSIZE);
  jelio_utostrf(buf+JELIOSIZE, l, i, 10, spacefill, zerofill);
  return (jelio*) buf;
}
