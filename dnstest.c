#include "jelio.h"

main(int argc, char **argv)
{
  int err=0, count, i;
  in_addr_t srv, srvs[4];
  char *qname;

  srv = sysdnsserv4(NULL, 0, NULL, &err);
  buprintv(1, "main server: %% %%\n", x(srv), ip4(srv));
  
  srv = sysdnsserv4(srvs, 4, &count, &err);
  for(i=0;i<count;i++)
    buprintv(1, "nameserver: %% %%\n", x(srvs[i]), ip4(srvs[i]));

  if(argc > 1) qname = argv[1];
  else qname = "mail1.slu.se";
  dnsfwd4(NULL, 0, qname, srv, 2000, NULL);

}
