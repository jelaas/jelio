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
#include <stdlib.h>

static struct jelio_buf *inputbuffers;

struct jelio_buf *jelio_buffer(int fd)
{
  struct jelio_buf *buf;
  for(buf=inputbuffers;buf;buf=buf->next)
    if(buf->fd == fd)
      return buf;
  buf = malloc(sizeof(struct jelio_buf));
  if(!buf) return NULL;
  buf->fd = fd;
  buf->len = 0;
  buf->pos = 0;
  buf->eof = 0;
  buf->buf = malloc(JELIO_IBUFSIZE);
  if(!buf->buf) return NULL;
  buf->next = inputbuffers;
  inputbuffers = buf;
  return buf;
}
struct jelio_buf **jelio_inputbuffers(void)
{
  return &inputbuffers;
}

