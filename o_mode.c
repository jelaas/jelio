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

jelio *o_mode(mode_t mode)
{
  char *buf;
  /*                digits           sign 0 NULL */
  int l =  (sizeof(mode_t)*9) +1 +1 +1 +1;
  buf = malloc(l + JELIOSIZE);

  jelio_protcpy(buf, buf+JELIOSIZE);
  jelio_llutostr(buf+JELIOSIZE+1, l-1, mode, 8);
  buf[JELIOSIZE] = '0';
  return (jelio*) buf;
}
