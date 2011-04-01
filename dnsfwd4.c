/*

rfc1035

Query:
----------
ID,
flags: RD,RCODE,QDCOUNT,ANCOUNT,NSCOUNT,ARCOUNT
QNAME field: fill with fqdn
QTYPE
----------

Response:
----------
ID,
Number of answer records
NAME
TYPE CLASS TTL RDLENGTH
RDATA
----------

      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
ID              A 16 bit identifier assigned by the program 
QR              A one bit field that specifies whether this message is a
                query (0), or a response (1).
OPCODE          A four bit field that specifies kind of query in this
                message.  This value is set by the originator of a query
                and copied into the response.  The values are:
                0               a standard query (QUERY)
                1               an inverse query (IQUERY)
AA              Authoritative Answer - this bit is valid in responses,
                and specifies that the responding name server is an
                authority for the domain name in question section.
TC              TrunCation - specifies that this message was truncated
RD              Recursion Desired - this bit may be set in a query and
                is copied into the response.  If RD is set, it directs
                the name server to pursue the query recursively.
                Recursive query support is optional.
RA              Recursion Available - this be is set or cleared in a
                response, and denotes whether recursive query support is
                available in the name server.
Z               Reserved for future use. 0.
RCODE           Response code - this 4 bit field is set as part of
                responses.


DNS strings:
create a register of all the strings:
 index, zlen, "realstring"

UDP packet maxsize 512 bytes.
TCP packet prefixed by short int with message length.


*/

#include "jelio.h"
#include <stdlib.h>
#include <string.h>

struct dns_pkt {
  unsigned short id;
  unsigned char rd, rcode;
  unsigned short qdcount, ancount, nscount, arcount;

  char *qname;
  char *qclass, *qtype;
  
};

struct dnsstring {
  int index; /* index >= p < index + zlen */
  int zlen;
  char *string;
};

struct dnsstrreg {
  unsigned char *buf;
  size_t bufsize;
  struct dnsstring **strings;
  int nstrings;
  int mstrings;
};

int dnsstrreg_init(struct dnsstrreg *reg, unsigned char *buf, size_t bufsize)
{
  reg->buf = buf;
  reg->bufsize = bufsize;
  
  reg->mstrings = 8;
  reg->nstrings = 0;
  reg->strings = malloc(sizeof(struct dnsstring *)*reg->mstrings);
  
  return 0;
}

int dnsstrreg_free(struct dnsstrreg *reg)
{
  int i;
  
  if(reg->strings)
    {
      for(i=0;i<reg->nstrings;i++)
	free(reg->strings[i]);
      
      free(reg->strings);
    }
  return 0;
}

/*
 * convert qname to dnsstring and write it to buf.
 * reg is used to remember previously written strings to the buffer.
 */
unsigned char *strtodns(struct dnsstrreg *reg, char *qname, int *len)
{
  unsigned char buf[256], *p, *rc;
  char *qp;
  int plen, zlen; /* compressed length of string */
  
  p = buf;
  qp = qname;

  /* loop over each domain level part */
  while(*qp)
    {
      if(strchr(qp, '.'))
	plen = strchr(qp, '.') - qp;
      else
	plen = strlen(qp);
      
      /* if the rest of the name is found in registry enter a pointer */
      ;
      
      /* write this part */
      *p++ = plen;
      while(plen--)
	*p++ = *qp++;
      if(*qp == '.')
	qp++;
    }
  zlen = p - buf;
  
  if(len) *len = zlen;
  
  rc = malloc(zlen);
  memcpy(rc, buf, zlen);
  return rc;
}

int dnsstrreg_add(struct dnsstrreg *reg, char *str, unsigned char *pos, int zlen)
{
  struct dnsstring *ds, **va;
  int oldsize;

  ds = malloc(sizeof(struct dnsstring));

  ds->index = pos-reg->buf;
  ds->zlen = zlen;
  ds->string = str;
  
  reg->strings[reg->nstrings++] = ds;
  if(reg->nstrings >= reg->mstrings)
    {
      oldsize = reg->mstrings;
      reg->mstrings += 8;
      va = realloc(reg->strings,
		   sizeof(struct dnsstring *)*reg->mstrings);
      memcpy(va, reg->strings, sizeof(struct dnsstring *)*oldsize);
      reg->strings = va;
    }
  return 0;
}

/* QNAME, QTYPE, QCLASS */
/* unpack a query section record. */
int dns_qr_unpack(struct dns_qr *opt_res, unsigned char *qr, int len)
{
  return 0;
}

in_addr_t *dnsfwd4(in_addr_t *opt_res,
		   int opt_maxres,
		   char *fqdn,
		   in_addr_t dnsserv,
		   int timeout_ms,
		   int *opt_err)
{
  struct dnsstrreg reg;
  unsigned char pkt[1500];
  unsigned char *recv, *p, *ds;
  int pktsize, recvsize;
  int err=0;
  int dslen;
  int qdcount,ancount,nscount,arcount;

  unsigned int id = (time(0)+getpid()) & 0xffff;
  
  if(opt_err && *opt_err) return NULL;

  dnsstrreg_init(&reg, pkt, 1500);
  
  p = pkt;
  *p++ = id >> 8;
  *p++ = id & 255;
  *p++ = 1; /* rd bit: recursion desired*/
  *p++ = 0; /* rcode */
  *p++ = 0; /* qdcount high */
  *p++ = 1; /* qdcount low */
  *p++ = 0; /* ancount hi */
  *p++ = 0; /* ancount lo */
  *p++ = 0; /* nscount hi */
  *p++ = 0;
  *p++ = 0; /* arcount hi */
  *p++ = 0;
  
  /* write qname as dns_string */
  ds = strtodns(&reg, fqdn, &dslen);
  memcpy(p, ds, dslen);
  dnsstrreg_add(&reg, fqdn, p, dslen);
  free(ds);
  p += dslen;
  *p++ = 0; /* NULL terminate - empty string */
  
  *p++ = 0; /* A hi */
  *p++ = 1; /* A low */
  *p++ = 0; /* IN hi */
  *p++ = 1; /* IN low */
  
  dnsstrreg_free(&reg);
  
  pktsize = p-pkt;
  
  /* send query */
  
  recv = dnsquery4(dnsserv, pkt, pktsize, &recvsize, timeout_ms, &err);
  if(recv)
    {
      buprintv(1, "we got an answer of size %%\n", d(recvsize));
      /* parse response */

      p = recv;
      if( (*p << 8) + (*(p+1)) == id)
	buprintv(1, "id %% matches\n", u(id));
      p+=2; /* id */

      if(*p & 128) buprintv(1, "QR=1: DNS response\n");
      if( (*p & (64+32+16+8)) == 0) buprintv(1, "Opcode: QUERY\n");
      if( (*p & (64+32+16+8)) == 1) buprintv(1, "Opcode: IQUERY\n");
      if( (*p & (64+32+16+8)) == 2) buprintv(1, "Opcode: STATUS\n");
      if(*p & 4) buprintv(1, "AA=1: authoritative answer\n");
      if(*p & 2) buprintv(1, "TC=1: truncated\n");
      p+=2; /* rd rcode */

      qdcount = *p++ << 8;
      qdcount += *p++;
      buprintv(1, "qdcount=%%\n", d(qdcount));

      ancount = *p++ << 8;
      ancount += *p++;
      buprintv(1, "ancount=%%\n", d(ancount));

      nscount = *p++ << 8;
      nscount += *p++;
      buprintv(1, "nscount=%%\n", d(nscount));

      arcount = *p++ << 8;
      arcount += *p++;
      buprintv(1, "arcount=%%\n", d(arcount));

      if(qdcount)
	dns_qr_unpack(NULL,NULL,0);
    }
  else
    buprintv(1, "we got no answer\n");
  
  return NULL;
}
