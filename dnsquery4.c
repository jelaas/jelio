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

/* send query to dnsserv.
   if pktsize is less then 512 bytes try a udp query first, else tcp.
   if we get no answer after two udp queries try tcp.
*/

unsigned char *dnsquery4(in_addr_t dnsserv,
			 unsigned char *pkt, int pktsize,
			 int *recvsize, int timeout_ms, int *opt_err)
{
  int err=0;
  int udp_tries=0;
  unsigned char *recv;

  if(opt_err && *opt_err) return NULL;
  
  if(pktsize <= 512) udp_tries=2;
  
  while(udp_tries--)
    {
      buprintv(1, "udp try %% [%%]\n", d(udp_tries),ip4(dnsserv));
      
      recv = dnsquery4udp(dnsserv, pkt, pktsize, recvsize, timeout_ms, &err);
      if(recv) return recv;
      if(err) break;
      usleep(200000); /* 20 ms */
    }
  
  buprintv(1, "tcp try [%%]\n",ip4(dnsserv));

  recv = dnsquery4tcp(dnsserv, pkt, pktsize, recvsize, timeout_ms, &err);
  if(err)
    {
      if(opt_err) *opt_err = -1;
    }
  return recv;
}
