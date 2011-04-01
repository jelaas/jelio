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

struct jelbs *bsdup(struct jelbs *s)
{
  struct jelbs *ds;
  ds = bsopen(s->mem, s->size);
  ds->bytepos = s->bytepos;
  ds->bitpos = s->bitpos;
  ds->eof = s->eof;
  return ds;
}
