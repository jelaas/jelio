/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2006
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "jelio.h"
#include "jelio_internal.h"

#define PATH_RESCONF "/etc/resolv.conf"

in_addr_t sysdnsserv4(in_addr_t *opt_res, int opt_maxres, int *opt_count, int *opt_err)
{
  int fd, eof=0, count=0;
  in_addr_t first=0, ns;
  char buf[64];
  unsigned int i1,i2,i3,i4;

  if(opt_count) *opt_count = 0;
  
  fd = open(PATH_RESCONF, O_RDONLY);
  if(!fd)
    {
      if(opt_err) *opt_err = 1;
      return 0;
    }
  
  while(!eof)
    {
      if(bugets(fd, &eof, sizeof(buf), buf)==0)
	break;
      
      if(sscanv(buf, NULL, "nameserver%%%%.%%.%%.%%",
		i_set(NULL, 64, " \t", NULL),
		i_u(&i1), i_u(&i2), i_u(&i3), i_u(&i4)) == 5)
	{
	  ns = i1 << 24;
	  ns |= i2 << 16;
	  ns |= i3 << 8;
	  ns |= i4;
	  ns = htonl(ns);
	  if(!first) first = ns;
	  if(opt_res && (count < opt_maxres)) *opt_res++ = ns;
	  count++;
	}
    }
  buclose(fd);
  close(fd);
  if(!first)
    if(opt_err) *opt_err = 2;
  if(opt_count) *opt_count = count;
  return first;
}
