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

void i_default_free(struct jelio_input *ji)
{
  if(ji->free)
    ji->free(ji);
  else
    free(ji);
}
