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

int xxprintv_push_fd(int dstkey, char *opt_prefix, int fd, size_t maxlen)
{
  struct xcontext *c = malloc(sizeof(struct xcontext));
  if(!c) return -1;
  memset(c, 0, sizeof(struct xcontext));
  c->fd = fd;
  c->maxlen = maxlen-1;
  c->print = vabuprintv;

  c->prefix = opt_prefix;

  return jelio_xcontext_push(dstkey, c);
}
