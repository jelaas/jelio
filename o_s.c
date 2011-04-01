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

jelio *o_s(const char *s)
{
  char *buf;
  if(!s) return NULL;
  buf = malloc(JELIOSIZE);
  if(!buf) return NULL;
  
  jelio_protcpy(buf, s);
  return (jelio*) buf;
}
