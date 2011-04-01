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

 /* char or EOF */
int bugetc(int fd)
{
  struct jelio_buf *jb;
  
  jb = jelio_buffer(fd);
  if(!jb) return -2;
  return jelio_buffer_getc(jb);
}
