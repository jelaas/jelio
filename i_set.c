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

struct _i_set {
  char *val;
  const char *memb;
  const char *non_memb;
};

/*
 * FIXME: support ranges [0-9] etc. support [[-]
 *  or 0-9 + [chars] [[] []]
 */
static inline int _i_set_member(const char *set, int c)
{
  return strchr(set, c)?1:0;
}

static int i_set_conv(void *value, int n, void *inbuf)
{
  struct jelio_buf *jb = inbuf;
  struct _i_set *m = value;
  char *sval;
  int c,count=0;
  int scount=0;
  
  sval = m->val;
  
  if(n < 1) return 0;
  n--;
  
  while(1)
    {
      if(scount >= n)
	break;
      c = jelio_buffer_peekc(jb);
      if(c == -1)
	break;
      if(m->non_memb && _i_set_member(m->non_memb,c))
	break;
      if(m->memb && (!_i_set_member(m->memb,c)) )
	break;
      count++;
      scount++;
      jelio_buffer_getc(jb);
      if(sval) *sval++ = c;
    }
  if(sval) *sval = 0;
  return count;
}

static void i_set_free(struct jelio_input *ji)
{
  free(ji->data);
  free(ji);
}

struct jelio_input *i_set(char *opt_s, int valsize,
			  const char *opt_memb, const char *opt_nonmemb)
{
  struct jelio_input *ji;
  struct _i_set *m;
  ji = malloc(sizeof(struct jelio_input));
  m = malloc(sizeof(struct _i_set));
  m->val = opt_s;
  m->memb = opt_memb;
  m->non_memb = opt_nonmemb;
  ji->data = m;
  ji->valsize = valsize;
  ji->callback = i_set_conv;
  ji->free = i_set_free;
  return ji;
}

