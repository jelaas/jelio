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
 * If the domain is AF_INET then src must point to a 4 byte in_addr_t in
 * network byte order (in_addr_t is a four byte unsigned integer).
 *
 * If the domain is AF_INET6 then src must point to a sequence of 16 unsigned
 * characters that make up the IPV6 address.
 *
 * Port is the portnumber for INET and INET6.
 *
 * If the domain is AF_UNIX or AF_LOCAL then src is a filesystem path.
 * If the path starts with a NULL (a zero length string) then it is assumed to be
 * a linux specific abstract socket (it has a name but is not mapped into the fs).
 */
int tcplisten(const unsigned char *src, int domain, int port)
{
  /* FIXME: perhaps break this function into three
     for more effective linking */

  union {
    struct sockaddr src;
    struct sockaddr_in src4;
    struct sockaddr_in6 src6;
    struct sockaddr_un srcun;
  } srca;
  socklen_t srcsize = 0;
  int s=-1;
  int one = 1;
  
  memset(&srca, 0, sizeof(srca));
  
  if((domain == AF_UNIX)||(domain == AF_LOCAL))
    {
      char *path = (char*) src;
      
      srcsize = sizeof(srca.srcun);
      
      srca.srcun.sun_family = AF_LOCAL;
      
      s = socket(srca.srcun.sun_family, SOCK_STREAM, 0);
      if(s < 0) return -1;
      
      if(*path)
	memcpy(srca.srcun.sun_path, path, strlen(path)+1);
      else
	/* names in the abstract namespace are not zero-terminated. */
	memcpy(srca.srcun.sun_path, path, strlen(path+1)+1);      
    }

  if(domain == AF_INET)
    {
      srcsize = sizeof(srca.src4);
      
      srca.src4.sin_family = AF_INET;

      s = socket(srca.src4.sin_family, SOCK_STREAM, 0);
      if(s < 0) return -1;
      
      if(src) srca.src4.sin_addr.s_addr = *(in_addr_t*)src;
      else srca.src4.sin_addr.s_addr = htonl(INADDR_ANY);
      srca.src4.sin_port = htons(port);
    }

  if(domain == AF_INET6)
    {
      srcsize = sizeof(srca.src6);
      
      srca.src6.sin6_family = AF_INET6;
      
      s = socket(srca.src6.sin6_family, SOCK_STREAM, 0);
      if(s < 0) return -1;
      
      if(src) memcpy(&srca.src6.sin6_addr.s6_addr, src, 16);
      else memcpy(&srca.src6.sin6_addr.s6_addr, &in6addr_any, 16);
      srca.src6.sin6_port = htons(port);
    }

  if(s >= 0)
    {
      setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
      
      /* try to bind it */
      if( bind(s, &srca.src, srcsize) == -1)
	{
	  close(s);
	  return -1;
	}
      
      if(listen(s,3) == -1)
	{
	  close(s);
	  return -1;
	}
    }
  
  return s;
}
