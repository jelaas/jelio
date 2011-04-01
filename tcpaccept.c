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

/*
 * If the domain is AF_INET then radr will point to a 4 byte in_addr_t in
 * network byte order (in_addr_t is a four byte unsigned integer).
 *
 * If the domain is AF_INET6 then radr will point to a sequence of 16 unsigned
 * characters that make up the IPV6 address.
 *
 * If the domain is AF_UNIX or AF_LOCAL then radr is a string of at most
 * UNIX_PATH_MAX (108) characters long. A buffer capable of holding UNIX_PATH_MAX+1
 * characters is thus needed. Note that abstract names may contain NULLS (and infact
 * start with a NULL character).
 */

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

static void utimediff(struct timeval *diff,
		      struct timeval *start,
		      struct timeval *stop)
{
  diff->tv_sec = stop->tv_sec - start->tv_sec;
  diff->tv_usec = stop->tv_usec - start->tv_usec;
  if(diff->tv_usec < 0)
    {
      diff->tv_sec--;
      start->tv_usec -= stop->tv_usec;
      diff->tv_usec = 1000000-start->tv_usec;
    }
}

int tcpaccept(int s, int opt_domain, unsigned char *opt_radr, int timeout_ms)
{
  int a, rc;
  int ms = timeout_ms;
  struct timeval timeout, timeleft, timenow;
  struct pollfd ufd;
  union {
    struct sockaddr radr;
    struct sockaddr_in radr4;
    struct sockaddr_in6 radr6;
    struct sockaddr_un radrun;
  } r;
  socklen_t radrsize;
  
  radrsize = sizeof(r);
  memset(&r.radr, 0, radrsize);

  ufd.fd = s;
  ufd.events = POLLIN;

  if(timeout_ms > 0)
    {
      gettimeofday(&timeout, NULL);
      timeout.tv_usec += (timeout_ms * 1000);
      while(timeout.tv_usec >= 1000000)
	{
	  timeout.tv_sec++;
	  timeout.tv_usec -= 1000000;
	}
    }

 do_accept:  
  a = accept(s, &r.radr, &radrsize);
  
  if(a == -1)
    {
      if((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR) )
	{
	  if(timeout_ms == 0)
	    return -1;
	  
	  if(timeout_ms > 0)
	    {
	      gettimeofday(&timenow, NULL);
	      utimediff(&timeleft, &timenow, &timeout);
	      ms = (timeleft.tv_sec*1000) + (timeleft.tv_usec/1000) ;
	      if(ms <= 0) return -1;
	    }
	  else
	    ms = -1;
	  rc = poll(&ufd, 1, ms);
	  if( ((rc == -1) && (errno != EINTR)) ||
	      ((rc > 0) && (ufd.revents & POLLERR)) )
	    return -1;
	  goto do_accept;
	}
      return -1;
    }
  
  if( (a >= 0) && opt_radr)
    {
      if((opt_domain == AF_UNIX)||(opt_domain == AF_LOCAL))
	{
	  if(r.radrun.sun_path[0])
	    memcpy(opt_radr, &r.radrun.sun_path, strlen(r.radrun.sun_path)+1);
	  else
	    memcpy(opt_radr, &r.radrun.sun_path, UNIX_PATH_MAX+1);
	}
      if(opt_domain == AF_INET)
	*((in_addr_t*)opt_radr) = r.radr4.sin_addr.s_addr;
      if(opt_domain == AF_INET6)
	memcpy(opt_radr, &r.radr6.sin6_addr.s6_addr, 16);
    }
  return a;
}
