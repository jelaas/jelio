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

unsigned char *dnsquery4tcp(in_addr_t dnsserv, unsigned char *pkt, int pktsize,
int *recvsize, int timeout_ms, int *opt_err)
{
   struct pollfd ufds;
  int err=0;
  int fd, got;
  unsigned char lbuf[2], *rc;

  if(opt_err && *opt_err) return NULL;

  fd = tcpopen((unsigned char *)&dnsserv, 53, NULL, 0, AF_INET, timeout_ms, &err);
  if(fd >= 0)
    {
      buprintv(1, "connected to %%\n", ip4(dnsserv));

      lbuf[0] = (pktsize >> 8);
      lbuf[1] = (pktsize & 255);
      write(fd, lbuf, 2);
      write(fd, pkt, pktsize);
      ufds.fd = fd;
      ufds.events = POLLIN;
      poll(&ufds, 1, timeout_ms);

      got = read(fd, lbuf, 2);
      close(fd);
      
      if(got == 2)
	{
	  pktsize = (lbuf[0] << 8) + lbuf[1];
	  rc = malloc(pktsize);
	  got = read(fd, rc, pktsize);
	  if(got > 0)
	    {
	      *recvsize = got;
	      return rc;
	    }
	  free(rc);
	}
    }
  return NULL;
}
