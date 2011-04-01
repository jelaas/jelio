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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int xprintv_push_dmem(char *opt_prefix, char **mem, size_t maxlen)
{
  struct xcontext *c = malloc(sizeof(struct xcontext));
  if(!c) return -1;
  memset(c, 0, sizeof(struct xcontext));
  c->fd = -1;
  c->dbuf = mem;
  c->maxlen = maxlen;
  c->maxlen--;
  c->print = vasbuprintv;

  c->prefix = opt_prefix;

  return jelio_context_push(c);
}


