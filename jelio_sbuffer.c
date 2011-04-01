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

struct jelio_buf *jelio_sbuffer(struct jelio_buf *buf, unsigned char *str)
{
  buf->fd = -1;
  buf->len = strlen((char *)str);
  buf->pos = 0;
  buf->eof = 1;
  buf->buf = str;
  buf->next = 0;
  return buf;
}

