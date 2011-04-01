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

/*
 * If the domain is AF_INET then src and dst must point to a 4 byte in_addr_t in
 * network byte order (in_addr_t is a four byte unsigned integer).
 *
 * If the domain is AF_INET6 then src and dst must point to a sequence of 16 unsigned
 * characters that make up the IPV6 address.
 *
 * If the domain is AF_UNIX or AF_LOCAL then src and dst are filesystem paths.
 * If the path starts with a NULL (a zero length string) then it is assumed to be
 * a linux specific abstract socket (it has a name but is not mapped into the fs).
 */
int tcpopen(const unsigned char *dst, int dst_port, const unsigned char *opt_src, int src_port, int domain, int timeout_ms, int *opt_err)
{
  /* FIXME: perhaps break this function into three
     for more effective linking */

  union {
    struct sockaddr src;
    struct sockaddr_in src4;
    struct sockaddr_in6 src6;
    struct sockaddr_un srcun;
  } srca;
  union {
    struct sockaddr dst;
    struct sockaddr_in dst4;
    struct sockaddr_in6 dst6;
    struct sockaddr_un dstun;
  } dsta;

  socklen_t dstsize = 0;
  int s=-1, rc;
  int flags;
  struct pollfd ufd;
  struct timeval timeout, timeleft, timenow;
  int ms = timeout_ms;
  
  if(opt_err && *opt_err) return -1;
  
  memset(&srca, 0, sizeof(srca));
  memset(&dsta, 0, sizeof(dsta));

  if((domain == AF_UNIX)||(domain == AF_LOCAL))
    {
      char *path;
      
      dstsize = sizeof(dsta.dstun);
      
      srca.srcun.sun_family = AF_LOCAL;
      
      s = socket(srca.srcun.sun_family, SOCK_STREAM, 0);
      if(s < 0) return -1;
      
      if(opt_src)
	{
	  path = (char*) opt_src;
	  if(*path)
	    memcpy(srca.srcun.sun_path, path, strlen(path)+1);
	  else
	    /* names in the abstract namespace are not zero-terminated. */
	    memcpy(srca.srcun.sun_path, path, strlen(path+1)+1);
	  
	  /* try to bind it */
	  if( bind(s,&srca.src, sizeof(srca.srcun)) == -1)
	    {
	      close(s);
	      return -1;
	    }
	}
      dsta.dstun.sun_family = AF_LOCAL;
      
      path = (char*) dst;
      if(*path)
	memcpy(dsta.dstun.sun_path, path, strlen(path));
      else
	memcpy(dsta.dstun.sun_path, path, strlen(path+1)+1);
    }

  if(domain == AF_INET)
    {
      dstsize = sizeof(dsta.dst4);
      
      srca.src4.sin_family = AF_INET;
      
      s = socket(srca.src4.sin_family, SOCK_STREAM, 0);
      if(s < 0) return -1;
      
      if(opt_src)
	{
	  srca.src4.sin_addr.s_addr = *(in_addr_t*)opt_src;
	  srca.src4.sin_port = htons(src_port);
	  
	  /* try to bind it */
	  if( bind(s,&srca.src, sizeof(srca.src4)) == -1)
	    {
	      close(s);
	      return -1;
	    }
	}
      dsta.dst4.sin_family = AF_INET;
      dsta.dst4.sin_addr.s_addr = *(in_addr_t*)dst;
      dsta.dst4.sin_port = htons(dst_port);
    }

  if(domain == AF_INET6)
    {
      dstsize = sizeof(dsta.dst6);
      
      dsta.dst6.sin6_family = AF_INET6;
      
      s = socket(srca.src6.sin6_family, SOCK_STREAM, 0);
      if(s < 0) return -1;
      
      if(opt_src)
	{
	  memcpy(&srca.src6.sin6_addr.s6_addr, opt_src, 16);
	  srca.src6.sin6_port = htons(src_port);
	  
	  /* try to bind it */
	  if( bind(s,&srca.src, sizeof(srca.src6)) == -1)
	    {
	      close(s);
	      return -1;
	    }
	}
      dsta.dst6.sin6_family = AF_INET6;
      memcpy(&dsta.dst6.sin6_addr.s6_addr, dst, 16);
      dsta.dst6.sin6_port = htons(dst_port);
    }

  if(timeout_ms >= 0)
    {
      flags = fcntl(s, F_GETFL);
      fcntl(s, F_SETFL, flags | O_NONBLOCK);
    }
  
  ufd.fd = s;
  ufd.events = POLLOUT;

  /* when do we timeout? */
  gettimeofday(&timeout, NULL);
  timeout.tv_usec += (timeout_ms * 1000);
  while(timeout.tv_usec >= 1000000)
    {
      timeout.tv_sec++;
      timeout.tv_usec -= 1000000;
    }
  
 try_connect:
  gettimeofday(&timenow, NULL);
  utimediff(&timeleft, &timenow, &timeout);
  ms = (timeleft.tv_sec*1000) + (timeleft.tv_usec/1000) ;
  rc = connect(s, &dsta.dst, dstsize);
  
  if( (rc < 0) && (errno != EISCONN) )
    {
      if(ms <= 0) return -1;
      if( (errno == EALREADY) || (errno == EINPROGRESS) || (errno == EINTR))
	{
	  if(errno != EINTR)
	    {
	      gettimeofday(&timenow, NULL);
	      utimediff(&timeleft, &timenow, &timeout);
	      ms = (timeleft.tv_sec*1000) + (timeleft.tv_usec/1000) ;
	      rc = poll(&ufd, 1, ms);
	      if( ((rc == -1) && (errno != EINTR)) ||
		  ((rc > 0) && (ufd.revents & POLLERR)) )
		{
		  close(s);
		  return -1;
		}
	    }
	  
	  /* calculate time left */
	  goto try_connect;
	}

      /* catch all other errors like ETIMEDOUT etc */
      close(s);
      return -1;
    }
  
  /* turn off NONBLOCKING mode */
  if(timeout_ms >= 0)
    {
      flags = fcntl(s, F_GETFL);
      fcntl(s, F_SETFL, flags & (~ O_NONBLOCK));
    }
    
  return s;
}
