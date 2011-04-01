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

off_t bsseek(struct jelbs *s, off_t offset, int whence)
{
  switch(whence)
    {
    case SEEK_SET:
      s->bytepos = offset / 8;
      s->bitpos = 7-(offset % 7);
      break;
    case SEEK_CUR:
      s->bytepos += offset / 8;
      s->bitpos -= (offset % 7);
      break;
    case SEEK_END:
      s->bytepos = s->size + offset / 8;
      s->bitpos = 0-(offset % 7);      
      break;
    default:
      return -1;
    }
  if(s->bitpos < 0)
    {
      s->bitpos += 8;
      s->bytepos++;
    }
  if(s->bitpos > 7)
    {
      s->bitpos -= 8;
      s->bytepos--;
    }
  if(s->bytepos < 0 || s->bytepos >= s->size)
    {
      s->eof=1;
      s->bytepos=0;
    }
  return (s->bytepos << 3) + (7-s->bitpos);
}

