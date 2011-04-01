/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2006
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


jelio *o_ip4(in_addr_t ipadr)
{
  char *buf = malloc( 24 +JELIOSIZE );
  
  jelio_protcpy(buf, buf+JELIOSIZE);
  *(buf+JELIOSIZE) = 0;

  ipadr = ntohl(ipadr);
  
  sprintv(buf+JELIOSIZE, 24, "%%.%%.%%.%%",
	  o_u(ipadr >> 24),
	  o_u((ipadr >> 16) & 255),
	  o_u((ipadr >> 8) & 255),
	  o_u(ipadr & 255));
    
  return (jelio*) buf;
}
