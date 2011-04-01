#include <sys/types.h>
#include <unistd.h>
#include "jelio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int rmem(unsigned int *vsize, unsigned int *rss, pid_t pid)
{
  int fd, n;
  char *mstr;
  char buf[256], *p;
  
  if(pid == -1)
    pid = getpid();
  
  sprintv_dup(&mstr, 1024, "/proc/%%/stat", pid(pid));
  
  fd = open(mstr, O_RDONLY);
  if(fd < 0)
    {
      buprintv(2, "ERR open %%\n", s(mstr));
      exit(1);
    }
  
  if(bugets(fd, NULL, sizeof(buf), buf) < 8)
    {
      buprintv(2, "ERR read %%\n", s(mstr));
      exit(1);
    }
  
  /* 22 spaces */
  p = buf;
  for(n=0;n<22;n++)
    {
      while(*p && *p != ' ')
	p++;
      if(*p) p++;      
    }

  *vsize = atoi(p);
  while(*p && *p != ' ')
    p++;
  if(*p) p++;
  *rss = atoi(p);
  
  buclose(fd);
  close(fd);
  return 0;
}

void leak(int count)
{
  char *buf="a x8";
  char vs[8];
  int v1,v2,v3;
  int i, n;

  for(i=0;i<count;i++)
    {
#if 1
      n = sscanv(buf, NULL, "%% %% %% %%", i_d(&v1), i_d(&v2), i_d(&v3),
		 i_set(vs, sizeof(vs), NULL, NULL));
#endif
#if 0
      n = sscanv(buf, NULL, "%% %% %%", i_d(&v1), i_d(&v2), i_d(&v3));
#endif
#if 0
      n = sscanv(buf, NULL, "%%", i_s(vs, sizeof(vs)));
#endif
    }
}

int main(int argc, char **argv)
{
  unsigned int vsize, rss;
  
  if(argc > 1)
    {
      rmem(&vsize, &rss, atoi(argv[1]));
      buprintv(1, "%%: vsize=%% rss=%%\n", d(atoi(argv[1])),
	       u(vsize), u(rss));
      exit(0);
    }

  leak(1000);
  rmem(&vsize, &rss, -1);
  rmem(&vsize, &rss, -1);
  buprintv(1, "START: vsize=%% rss=%%\n",
	   u(vsize), u(rss));

  leak(60000);

  rmem(&vsize, &rss, -1);
  buprintv(1, "MID: vsize=%% rss=%%\n",
	   u(vsize), u(rss));

  leak(60000);
  
  rmem(&vsize, &rss, -1);
  buprintv(1, "STOP: vsize=%% rss=%%\n",
	   u(vsize), u(rss));
  
  return 0;
}
