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

off_t buseek(int fd, off_t offset, int whence)
{
  struct jelio_buf *jb;

  jb = jelio_buffer(fd);
  return jelio_buffer_seek(jb, offset, whence);
}
