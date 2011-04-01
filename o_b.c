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

jelio *o_b(int boolean)
{
  char *buf;
  static char *s_true = "true";
  static char *s_false = "false";
  buf = malloc(JELIOSIZE);
  if(boolean)
    jelio_protcpy(buf, s_true);
  else
    jelio_protcpy(buf, s_false);
  return (jelio*) buf;
}
