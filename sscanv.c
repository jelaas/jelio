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
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

int sscanv(const char *buf, int *opt_consumed, const char *fmt, ...)
{
  struct jelio_buf jb;
  int count, eof=0, failed=0;
  va_list ap;
  
  jelio_sbuffer(&jb, (unsigned char *)buf);
  va_start(ap, fmt);
  count = vascanv(&jb, opt_consumed, &eof, &failed, fmt, ap);
  va_end(ap);
  if(eof && (count == 0) && failed) return -1;
  return count;
}
