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

int bseof(struct jelbs *s)
{
  if(s->bytepos > s->size) s->eof=1;
  if(s->bytepos < 0) s->eof=-1;
  if(s->eof)
    s->bytepos=0;
  return 0;
}
