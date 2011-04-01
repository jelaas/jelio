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

/*
 * base 'a' and 'A' are special bases that mean base 16 with
 *  lower or uppercase hexcharacters.
 *
 */

int jelio_utostr(char *s, unsigned int size, unsigned int i, unsigned int base)
{
  char *tmp;
  unsigned int j=0;
  unsigned int hexbase = 'A';
  
  s[--size]=0;

  tmp=s+size;

  if((base == 'A')||(base == 'a'))
    {
      hexbase = base;
      base = 16;
    }
  if((base==0)||(base>36)) base=10;
  
  j=0;

  if (!i)
  {
    *(--tmp)='0';
    j=1;
  }

  while((tmp>s)&&(i))
  {
    tmp--;
    if ((*tmp=i%base+'0')>'9') *tmp+=(hexbase)-'9'-1;
    i=i/base;
    j++;
  }
  memmove(s,tmp,j+1);

  return j;
}

