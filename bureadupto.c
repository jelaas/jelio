/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */

#include <string.h>
#include "jelio.h"
#include "jelio_internal.h"

/*
  Upto "upto" EOF maxlen.
  NULL term.
  "upto" stored in buffer if encountered.
   Overflow is left unread.
   eof marked if EOF encountered.
   -1 on error.
   upto_len == 0, upto_len = strlen(upto)
*/

int bureadupto(int fd, int *opt_eof, int maxlen, int *opt_complete, char *rbuf,
	       char *upto, int upto_len)
{
  int count=0;
  int buf;
  struct jelio_buf *jb;

  if(opt_eof) *opt_eof = 0;
  if(opt_complete) *opt_complete = 0;
  if(!rbuf) return -1;
  if(maxlen<2)
    {
      *rbuf = 0;
      return 0;
    }
  maxlen--;

  if(upto_len == 0) upto_len = strlen(upto);
  
  jb = jelio_buffer(fd);
  if(!jb) return -1;

  while(1)
    {
      buf = jelio_buffer_getc(jb);
      if(buf == -1)
	{
	  if(opt_eof) *opt_eof = 1;
	  break;
	}
      *(rbuf+count) = buf;
      count++;
      
      if(count >= upto_len)
	if(!memcmp(rbuf+count-upto_len, upto, upto_len))
	  {
	    if(opt_complete) *opt_complete = 1;
	    break;
	  }

      if(count >= maxlen)
	{
	  int i;
	  /*
	    check if tail of buffer corresponds with head of "upto".
	    if so unget that part of the tail.
	   */
	  if(upto_len >= count) upto_len = count-1;
	  if(count > 1)
	    for(i=upto_len;i>0;i--)
	      if(!memcmp(rbuf+count-i, upto, i))
		{
		  //printv("unget <%%> len %%\n", sub(rbuf, count-i, i), d(i));
		  jelio_buffer_ungetc(jb, rbuf+count-i, i);
		  count -= i;
		}
	  break;
	}
    }
  *(rbuf+count) = 0;
  return count;
}
