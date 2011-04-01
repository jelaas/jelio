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

/* cleanup! */
int jelio_sbuffer_close(struct jelio_buf *buf)
{
  if(buf)
    {
      free(buf);
      return 0;
    }
  return -1;
}


