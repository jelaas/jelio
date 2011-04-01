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
#include <string.h>

int jelio_utostrf(char *s, unsigned int size, unsigned int i,
		  unsigned int base,
		  int spacefill, int zerofill)
{
  char *tmp;
  unsigned int j=0;
  
  s[--size]=0;

  tmp=s+size;

  if ((base==0)||(base>36)) base=10;

  j=0;

  if (!i)
  {
    *(--tmp)='0';
    j=1;
  }

  while((tmp>s)&&(i))
  {
    tmp--;
    if ((*tmp=i%base+'0')>'9') *tmp+=('A')-'9'-1;
    i=i/base;
    j++;
  }
  zerofill -= j;
  while( (zerofill>0) && (tmp>s))
  {
    zerofill--;
    tmp--;
    *tmp='0';
    j++;
  }
  spacefill -= j;
  while( (spacefill>0) && (tmp>s))
  {
    spacefill--;
    tmp--;
    *tmp=' ';
    j++;
  }

  memmove(s,tmp,j+1);

  return j;
}

