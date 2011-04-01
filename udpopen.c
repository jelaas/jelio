/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>

#include "jelio.h"
#include "jelio_internal.h"

#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif
#ifndef AF_UNIX
#define AF_UNIX AF_LOCAL
#endif

int udpopen(int domain, int *opt_err)
{
  int s;
  
  s = socket(domain, SOCK_DGRAM, IPPROTO_UDP);
  if(s < 0) return -1;
  return s;
}
