#define _ISOC99_SOURCE
#include <unistd.h>
#include <stdarg.h>

/* Runtime protection */
#define JELIOSIZE sizeof(struct jelio)

#ifndef JELIO_NOPROTECT
#define JELIOPROTN 4
#define JELIOPROTS "$@10"
#include <string.h>
static inline void jelio_prot(char *str)
{ if(strncmp(str, JELIOPROTS,JELIOPROTN)) _exit(3);}
static inline void jelio_protcpy(char *buf, const char *str)
{ strncpy(buf, JELIOPROTS, JELIOPROTN);
  ((jelio*)buf)->data = str; }
#else
#define JELIOPROTN 0
#define JELIOPROTS ""
  static inline void jelio_prot(char *str){}
  static inline void jelio_protcpy(char *buf, const char *str)
 { ((jelio*)buf)->data = str; }
#endif

int jelio_itostr(char *s, unsigned int size, int i, unsigned int base);
int jelio_litostr(char *s, unsigned int size, long int i, unsigned int base);
int jelio_llitostr(char *s, unsigned int size, long long int i,
		   unsigned int base);
int jelio_utostr(char *s, unsigned int size, unsigned int i,
		 unsigned int base);
int jelio_lutostr(char *s, unsigned int size, unsigned long int i,
		  unsigned int base);
int jelio_llutostr(char *s, unsigned int size, unsigned long long int i,
		   unsigned int base);
int jelio_itostrf(char *s, unsigned int size, int i, unsigned int base,
		  int spacefill, int zerofill);
int jelio_utostrf(char *s, unsigned int size, unsigned int i,
		  unsigned int base,
		  int spacefill, int zerofill);
struct xcontext {
  int fd;
  char *buf;
  char **dbuf;
  size_t ulen, mlen, maxlen;
  struct xcontext *next;
  int (*print)(struct xcontext *c, const char *fmt, va_list ap);
  char *prefix;
};

#define JELIO_IBUFSIZE 1024
struct jelio_buf {
  int fd;
  unsigned char *buf;
  int len; /* chars in buffer */
  int pos; /* next char to read from buffer */
  int eof; /* 1 == nothing more to read */
  struct jelio_buf *next;
};
int vascanv(struct jelio_buf *jb, int *opt_cons, int *eof, int *failed, const char *fmt, va_list ap);
void i_default_free(struct jelio_input *ji);
struct jelio_buf *jelio_buffer(int fd);
struct jelio_buf *jelio_sbuffer(struct jelio_buf *jb, unsigned char *str); /* shortlived conv buffer */
int jelio_sbuffer_close(struct jelio_buf *buf);
struct jelio_buf **jelio_inputbuffers(void);
int jelio_buffer_getc(struct jelio_buf *jbuf); /* char or EOF */
int jelio_buffer_ungetc(struct jelio_buf *jbuf, char *buf, int n);
int jelio_buffer_peekc(struct jelio_buf *jbuf); /* char or EOF */
off_t jelio_buffer_seek(struct jelio_buf *jbuf, off_t offset, int whence);
ssize_t jelio_buffer_read(struct jelio_buf *jbuf,
			  void *buf,
			  size_t count); /* count or -1 */
int vabuprintv(struct xcontext *c, const char *fmt, va_list ap);
int vasbuprintv(struct xcontext *c, const char *fmt, va_list ap);
int jelio_context_push(struct xcontext *c);
int jelio_xcontext_push(int dstkey, struct xcontext *c);

int char_isspace(int c);

