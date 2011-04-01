/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004-2006
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include "jelio.h"
#include "jelio_internal.h"

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int xxprintv_push_null(int dstkey)
{
  struct xcontext *c = malloc(sizeof(struct xcontext));
  if(!c) return -1;
  memset(c, 0, sizeof(struct xcontext));
  c->fd = -1;
  c->maxlen = 0;
  c->print = NULL;
  c->prefix = NULL;

  return jelio_xcontext_push(dstkey, c);
}
