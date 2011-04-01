/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <stdlib.h>
#include <string.h>

#include "jelio.h"
#include "jelio_internal.h"

int jelio_buffer_ungetc(struct jelio_buf *jbuf, char *buf, int n)
{
  unsigned char *b;
  
  if(jbuf->pos > n)
    {
      /* direct insert */
      // printv("direct insert at: %%\n", s(jbuf->buf+jbuf->pos));
      memcpy(jbuf->buf+jbuf->pos-n, buf, n);
      jbuf->pos -= n;
      return 0;
    }

  if( (n + (jbuf->len-jbuf->pos)) < JELIO_IBUFSIZE)
    {
      /* fits in buffer */
      memmove(jbuf->buf+n, jbuf->buf+jbuf->pos, jbuf->len-jbuf->pos);
      memcpy(jbuf->buf, buf, n);
      jbuf->len = n + (jbuf->len-jbuf->pos);
      jbuf->pos = 0;
      return 0;
    }

  /* need to allocate a larger buffer */
  b = malloc(n + (jbuf->len-jbuf->pos));
  if(!b) return -1;
  memcpy(b, buf, n);
  memcpy(b+n, jbuf->buf+jbuf->pos, jbuf->len-jbuf->pos);
  jbuf->len = n + (jbuf->len-jbuf->pos);
  jbuf->pos = 0;
  free(jbuf->buf);
  jbuf->buf = b;
  return 0;
}
