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

struct jelbs *bsopen(char *mem, size_t size)
{
  struct jelbs *s = malloc(sizeof(struct jelbs));
  s->mem = mem;
  s->size = size;
  s->bytepos = 0;
  s->bitpos = 7;
  s->eof = 0;
  return s;
}
