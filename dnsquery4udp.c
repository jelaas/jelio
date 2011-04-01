/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2006
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include <stdlib.h>
#include "jelio.h"
#include "jelio_internal.h"

/* send one query to dnsserv and wait for a response. */

unsigned char *dnsquery4udp(in_addr_t dnsserv, unsigned char *pkt, int pktsize, int *recvsize, int timeout_ms, int *opt_err)
{
  struct pollfd ufds;
  int err=0;
  int fd, got;
  struct sockaddr_in dnsserv_addr;
  unsigned char buf[1500], *rc;
  
  if(opt_err && *opt_err) return NULL;
  
  memset(&dnsserv_addr, 0, sizeof(dnsserv_addr));
  dnsserv_addr.sin_addr.s_addr = dnsserv;
  dnsserv_addr.sin_family = AF_INET;
  dnsserv_addr.sin_port = htons(53);
  
  fd = udpopen(AF_INET, &err);
  if(fd >= 0)
    {
      sendto(fd, pkt, pktsize, 0, (struct sockaddr*)&dnsserv_addr, sizeof(dnsserv_addr));
      ufds.fd = fd;
      ufds.events = POLLIN;
      if(poll(&ufds, 1, timeout_ms)>0)
	got = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL); /* FIXME: check from adr */
      else
	got = 0;
      close(fd);
      if(got > 0)
	{
	  rc = malloc(got);
	  memcpy(rc, buf, got);
	  *recvsize = got;
	  return rc;
	}
    }
  return NULL;
}
