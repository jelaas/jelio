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

jelio *o_svnt(const char **argv, const char *opt_separator)
{
  int argc;

  if(!argv) return NULL;
  
  for(argc=0;argv[argc];argc++)
    ;
  
  return o_sv(argc, argv, opt_separator);
}
