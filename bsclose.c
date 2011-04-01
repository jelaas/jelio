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

int bsclose(struct jelbs *s)
{
  s->mem = NULL;
  s->size = -1;
  s->bytepos = -1;
  s->bitpos = -1;
  free(s);
  return 0;
}
