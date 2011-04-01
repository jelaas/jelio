/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include "jeliof.h"
#include "jelio_internal.h"

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
  %% -> arg expected
  \  -> escape next character
*/

int printv(char *fmt, ...)
{
  int rc;
  va_list ap;
  va_start(ap, fmt);
  rc = vafprintv(stdout, fmt, ap);
  va_end(ap);
  return rc;
}
