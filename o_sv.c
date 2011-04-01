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

jelio *o_sv(int argc, const char **argv, const char *opt_separator)
{
  char *buf;
  int slen;
  int i, tot = 0;

  if(!argc) return NULL;

  if(opt_separator) slen = strlen(opt_separator); else slen=0;
  
  for(i=0; i<argc; i++)
    if(argv[i])
      tot += strlen(argv[i])+slen;
  
  buf = malloc(tot+1+JELIOSIZE);
  if(!buf) return NULL;
  jelio_protcpy(buf, buf+JELIOSIZE);
  *(buf+JELIOSIZE) = 0;
  for(i=0; i<argc; i++)
    {
      if(argv[i])
	{
	  strcat(buf+JELIOSIZE, argv[i]);
	  if( opt_separator && (i+1 < argc))
	    strcat(buf+JELIOSIZE, opt_separator);
	}
    }
  return (jelio*) buf;
}
