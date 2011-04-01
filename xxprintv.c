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

#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

struct xnode {
  int key;
  struct xcontext *item;
  struct xnode *next;
};

static struct xnode *xxlist = NULL;

static struct xnode *_xnode_find(int key)
{
  struct xnode *n;

  for(n=xxlist;n;n=n->next)
    {
      if(n->key == key)
	return n;
    }
  return NULL;
}

static struct xcontext *_xcontext_find(int key)
{
  struct xnode *n;
  struct xcontext *x;
  
  n = _xnode_find(key);
  if(n && n->item)
    {
      for(x=n->item;x->next;x=x->next)
	;
      return x;
    }
  return NULL;
}

int jelio_xcontext_push(int dstkey, struct xcontext *c)
{
  struct xnode *n;
  struct xcontext *x;
  
  x = _xcontext_find(dstkey);
  if(x)
    {
      x->next = c;
      return 0;
    }

  n = _xnode_find(dstkey);
  if(n)
    {
      n->item = c;
      return 0;
    }
  
  n = malloc(sizeof(struct xnode));
  n->key = dstkey;
  n->item = c;
  n->next = xxlist;
  xxlist = n;
  
  return 0;  
}

int xxprintv_isnull(int dstkey)
{
  struct xnode *n;
  if((n=_xnode_find(dstkey)) == NULL)
    return -1;
  if(n->item->print) return 0;
  return 1;
}

int xxprintv_pop(int dstkey)
{
  struct xnode *n;
  struct xcontext *x, *prev=NULL;
  
  if((n=_xnode_find(dstkey)) == NULL)
    return -1;
  
  if(!n->item)
    return -1;
  
  for(x=n->item;x->next;x=x->next)
    prev=x;
  
  if(prev)
    {
      prev->next = x->next;
      free(x);
      return 0;
    }

  n->item = NULL;
  free(x);
  return 0;
}

static int scanout(const char *fmt, va_list ap)
{
  /* need to free all string args */
  fmt++;
  while(*fmt)
    {
      if( (*fmt == '\\') &&
	  *(fmt+1) )
	{
	  fmt+=2;
	  continue;
	}
      if( (*fmt == '%') && 
	  (*(fmt+1) == '%') )
	{
	  char *s;
	  s = va_arg(ap, char*);
	  if(s) free(s);
	  fmt++;
	}
      fmt++;
    }
  return 0;
}

static struct xcontext _defaultcontext = { 2, NULL, NULL, 0, 0, 0, NULL, vabuprintv, NULL };

int xxprintv(int dstkey, const char *fmt, ... )
{
  int rc = 0;
  va_list ap;
  
  struct xcontext *x;

  x = _xcontext_find(dstkey);
  
  va_start(ap, fmt);
  if(!x)
    {
      rc = vabuprintv(&_defaultcontext, fmt, ap);
      va_end(ap);
      return rc;
    }
  else
    {
      if(x->print)
	rc = x->print(x, fmt, ap);
      else
	scanout(fmt, ap);
      va_end(ap);
      return rc;
    }
  return -1;
}
