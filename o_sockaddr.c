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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>


jelio *o_sockaddr(const struct sockaddr *adr)
{
  char *buf = malloc( 64 +JELIOSIZE );
  
  jelio_protcpy(buf, buf+JELIOSIZE);
  *(buf+JELIOSIZE) = 0;
  
  if(adr)
    {
      switch(adr->sa_family)
	{
	case AF_INET:
	  {
	    struct sockaddr_in *inaddr;
	    inaddr = (struct sockaddr_in*)adr;
	    strcpy(buf+JELIOSIZE, inet_ntoa(inaddr->sin_addr));
	  }
	  break;
	case AF_INET6:
	  {
	    struct sockaddr_in6 *inaddr;
	    inaddr = (struct sockaddr_in6*)adr;
	    inet_ntop(AF_INET6,
		      &inaddr->sin6_addr,
		      buf+JELIOSIZE,
		      64);
	  }
	  break;
	default:
	  break;
	}
    }
  
  return (jelio*) buf;
}
