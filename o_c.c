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

jelio *o_c(const int c)
{
  char *buf;
  buf = malloc(JELIOSIZE + 2);
  *(buf+JELIOSIZE) = (unsigned char) c;
  *(buf+JELIOSIZE+1) = 0;
  jelio_protcpy(buf, buf + JELIOSIZE);
  return (jelio*) buf;
}
