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

static int xcontext = 0;
static struct xcontext *xcontextptr = NULL;

int jelio_context_push(struct xcontext *c)
{
  c->next = xcontextptr;
  xcontextptr = c;
  xcontext = 1;
  return 0;  
}

int xprintv_pop(int *opt_fd, void *opt_mem, size_t *opt_maxlen)
{
  struct xcontext *c;
  if(xcontextptr)
    {
      c = xcontextptr->next;
      free(xcontextptr);
      xcontextptr = c;
      if(!c) xcontext = 0;
      return 0;
    }
  return -1;
}

static struct xcontext _defaultcontext = { 1, NULL, NULL, 0, 0, 0, NULL, vabuprintv, NULL };

int xprintv(const char *fmt, ... )
{
  int rc = 0;
  va_list ap;
  
  va_start(ap, fmt);
  switch(xcontext)
    {
    case 0:
      rc = vabuprintv(&_defaultcontext, fmt, ap);
      va_end(ap);
      return rc;
    case 1:
      rc = xcontextptr->print(xcontextptr, fmt, ap);
      va_end(ap);
      return rc;
    }
  return -1;
}
