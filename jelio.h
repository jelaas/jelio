#ifndef JELIO_H
#define JELIO_H

#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>

/* defines:

   Client compile time:
   JELIO_NOSHORTO  -- if defined turns off o_X -> X mapping 

   Lib compile time:
   JELIO_NOPROTECT -- turns off arg protection for vararg functions
                      (does _exit(2) when error occurs).
*/
/* #define JELIO_NOPROTECT */

/* struct used for typechecking and arg passing */
struct jelio {
#ifndef JELIO_NOPROTECT
 char mark[4];
#else
 char mark[0];
#endif
 const void *data; };
typedef struct jelio jelio;

/******************************
 * Output Conversion functions
 */

jelio *o_d(int i);
jelio *o_ld(long int i);
jelio *o_lld(long long int i);
jelio *o_dF(int i, int spacefill, int zerofill);
jelio *o_x(unsigned int i);
jelio *o_X(unsigned int i);
jelio *o_u(unsigned int i);
jelio *o_lu(unsigned long int i);
jelio *o_llu(unsigned long long int i);
jelio *o_uF(unsigned int i, int size, int zerofill);
jelio *o_c(const int c);
jelio *o_s(const char *s);
jelio *o_fs(char *s);
jelio *o_xs(const unsigned char *s, int n);
jelio *o_b64(const char *base64, size_t srclen);
jelio *o_qp(const char *qpstr, size_t srclen);
jelio *o_url(const char *urlstr, size_t srclen);
jelio *o_nstr(const char *s);
jelio *o_sub(const char *s, int offset, int len);
jelio *o_tr(const char *s, const char *from_ch, const char *opt_to_ch);
jelio *o_sv(int argc, const char **argv, const char *opt_separator);
jelio *o_svnt(const char **argv, const char *opt_separator);
jelio *o_t(time_t t);
jelio *o_uid(uid_t uid);
jelio *o_gid(gid_t gid);
jelio *o_pid(pid_t pid);
jelio *o_mode(mode_t mode);
jelio *o_at(time_t t); /* ascii format GMT: YYYY-MM-DD HH:MM:SS */
jelio *o_b(int boolean);
jelio *o_sockaddr(const struct sockaddr *adr);
jelio *o_ip4(in_addr_t ipadr);

/* Additional format specs: left/right position, space infront of positives,
   + infront of positives, base prefix(0,0x), precision, max field len

   Strings: printing of substrings: char *, offset, length
 */

/* TODO: o_p,o_P(pointer) o_f,o_e(floats)
   o_offset(offset_t), o_size(size_t),
   o_iftrue(boolean, jelio *)

   All output functions in callback variant:
   jelio_cb *oc_d(int)
   jelio_cb *oc_s(char *)
   cb_dprintv()
 */

#ifndef JELIO_NOSHORTO
#define d(D) o_d(D)
#define ld(D) o_ld(D)
#define lld(D) o_lld(D)
#define dF(I,S,Z) o_dF(I,S,Z)
#define x(U) o_x(U)
#define X(U) o_X(U)
#define u(U) o_u(U)
#define lu(U) o_lu(U)
#define llu(U) o_llu(U)
#define uF(I,S,Z) o_uF(I,S,Z)
#define s(S) o_s(S)
#define fs(S) o_fs(S)
#define xs(S,N) o_xs(S,N)
#define b64(S,N) o_b64(S,N)
#define qp(S,N) o_qp(S,N)
#define url(S,N) o_url(S,N)
#define nstr(S) o_nstr(S)
#define c(S) o_c(S)
#define sub(S,O,L) o_sub(S,O,L)
#define tr(S,F,T) o_tr(S,F,T)
#define sv(C,V,S) o_sv(C,V,S)
#define svnt(V,S) o_svnt(V,S)
#define t(T) o_t(T)
#define uid(U) o_uid(U)
#define gid(U) o_gid(U)
#define pid(U) o_pid(U)
#define mode(M) o_mode(M)
#define at(T) o_at(T)
#define b(B) o_b(B)
#define ip4(I) o_ip4(I)
#endif

/********************************
 * Basic output: %% = output arg
 */
ssize_t jelio_write(int fd, const void *buf, size_t count); /* stubborn */

/* *printv returns number of characters written or -1 for error.
   sprintv not counting terminating NULL.
 */
int dprintv(int fd, const char *fmt, ... );

 /* simple buffered version of dprintv */
int buprintv(int fd, const char *fmt, ... );

/* returns number of characters that would have been written to
   a destination */
int cprintv(const char *fmt, ...);

/* prints to string buffer. Result always NULL terminated */
int sprintv(char *buf, size_t maxlen, const char *fmt, ...);

/* return malloc'd string in *buf. maxlen is honored */
int sprintv_dup(char **buf, size_t maxlen, const char *fmt, ...);

/* appends to growing string buffer. Result always NULL terminated.
   If *buf is NULL an initial buffer is allocated.
   If *buf is non-NULL then: ulen must be the size of buffer used (ulen<mlen)
                             mlen must be the malloced size of the buffer.
   *buf, ulen and mlen is maintained between invocations.
   Returns nr of characters appended if successful.
           -1 if grow failed.
           -2 when maxlen reached.
 */
int sbuprintv(char **buf, size_t *ulen, size_t *mlen, size_t maxlen, const char *fmt, ...);

 /* append to file */
int lprintv(char *fn, mode_t mode, const char *fmt, ...);

/************************
 * Context based output
 */

/* print to destination governed by current context */
int xprintv(const char *fmt, ...);

/* push and create a new working context */
int xprintv_push_mem(char *opt_prefix, char *mem, size_t maxlen);
int xprintv_push_dmem(char *opt_prefix, char **mem, size_t maxlen);
int xprintv_push_fd(char *opt_prefix, int fd, size_t maxlen);
int xprintv_push_fn(char *opt_prefix, char *fn, mode_t mode);

/* pop and destroy current working context */
int xprintv_pop(int *opt_fd, void *opt_mem, size_t *opt_maxlen);

/* set prefix associated with current context */
char *xprintv_set_prefix(char *prefix);

/* print to destination governed by the current context asociated
   with the key 'dstkey' */
int xxprintv(int dstkey, const char *fmt, ...);

/* push and create a new working context associated with 'dstkey' */
int xxprintv_push_null(int dstkey);
int xxprintv_push_fd(int dstkey, char *opt_prefix, int fd, size_t maxlen);
int xxprintv_push_fn(int dstkey, char *opt_prefix, char *fn, mode_t mode);

/* pop and destroy current working context */
int xxprintv_pop(int dstkey);

/* set prefix associated with current context */
char *xxprintv_set_prefix(int dstkey, char *prefix);

/* ask if null context */
int xxprintv_isnull(int dstkey);

/* predefined dstkeys */
enum { _X_ASSERTION=-1, _X_PANIC=-2, _X_ERROR=-3, _X_WARNING=-4, _X_APPLICATION=-5,
       _X_INFO=-6, _X_DEBUG=-7 };

#define xx_assert(expr) \
 (expr) ? 0 : \
 xxprintv(_X_ASSERTION, "assertion %% failed in %% at %% in function %%\n",\
 s(__STRING(expr)), s(__FILE__), d(__LINE__), \
 s(__func__))

#define xx_error(fmt, args...)\
  do{if(xxprintv_isnull(_X_ERROR)!=1) xxprintv(_X_ERROR, fmt, ##args);} while(0)
#define xx_warn(fmt, args...)\
  do{if(xxprintv_isnull(_X_WARNING)!=1) xxprintv(_X_WARNING, fmt, ##args);} while(0)
#define xx_app(fmt, args...)\
  do{if(xxprintv_isnull(_X_APPLICATION)!=1) xxprintv(_X_APPLICATION, fmt, ##args);} while(0)
#define xx_info(fmt, args...)\
  do{if(!xxprintv_isnull(_X_INFO)) xxprintv(_X_INFO, fmt, ##args);} while(0)
#define xx_debug(fmt, args...)\
  do{if(!xxprintv_isnull(_X_DEBUG)) xxprintv(_X_DEBUG, fmt, ##args);} while(0)

struct jelio_input {
  void *data;
  int valsize;
  int (*callback)(void *, int, void *);
  void (*free)(struct jelio_input *);
  /*
    consumed = convert(*value, [valuesize], *jelbuf)
   */
};

/*****************************
 * Input Conversion functions
 */
struct jelio_input *i_d(int *opt_i);
struct jelio_input *i_ld(long int *i);
struct jelio_input *i_t(time_t *i);
struct jelio_input *i_pid(pid_t *i);
struct jelio_input *i_gid(gid_t *i);
struct jelio_input *i_uid(uid_t *i);
struct jelio_input *i_u(unsigned int *i);
struct jelio_input *i_lu(unsigned long int *i);
struct jelio_input *i_X(unsigned int *i);
struct jelio_input *i_lX(unsigned long int *i);

 /* limited conversion */
struct jelio_input *i_s(char *opt_s, int valsize);
struct jelio_input *i_set(char *opt_s, int valsize, const char *opt_memb, const char *opt_nonmemb);
struct jelio_input *i_xs(unsigned char *s, int valsize);
struct jelio_input *i_Xs(unsigned char *s, int valsize);

 /* conv to new malloced string */
struct jelio_input *i_sdup(char **s);
struct jelio_input *i_nstrdup(char **s);

/******************************
 * Basic input: %% = input arg
 */

ssize_t jelio_read(int fd, void *buf, size_t count, int *opt_err);

int bugetc(int fd);
int bupeekc(int fd);
int buscanv(int fd, int *opt_consumed, const char *fmt, ...);
int sscanv(const char *buf, int *opt_consumed, const char *fmt, ...);
int buread(int fd, int *opt_eof, int len, void *opt_buf);
off_t buseek(int fd, off_t offset, int whence);

/*
  Below:
  args prefixed with "opt_" are optional.
  if opt_buf is NULL everything read is lost, unless otherwise specified.
  Returns number of characters stored in buffer, not counting NULL term.
  
  bu* versions of functions are buffered. Do not mix with unbuffered!
*/

/*
 * Upto NULL, LF, EOF
 * LF or NULL not stored.
 * NULL-terminated.
 */
int dgetln(int fd, int *opt_eof, int maxlen, char *opt_buf);
int bugetln(int fd, int *opt_eof, int maxlen, char *opt_buf);
int bureadln(int fd, int *opt_eof, int maxlen, char *opt_buf);
int bugetdupln(int fd, int *opt_eof, int maxlen, char **opt_buf);

/* Cleanup for buffered input functions!
   Dont forget or strange things may happen!
   If you reuse the same fd the data remaining in buffer
   will be applied to the new fd.
   (Any data still in buffer will be lost!) */
int buclose(int fd);

 /*
   Upto LF EOF.
   NULL term.
   LF stored in buffer if encountered before maxlen reached.
   Overflow is lost.
   eof marked if EOF encountered.
 */
int dgets(int fd, int *opt_eof, int maxlen, char *opt_buf);
int bugets(int fd, int *opt_eof, int maxlen, char *opt_buf);

 /*
   Upto LF EOF.
   NULL term.
   LF stored in buffer if encountered before maxlen reached.
   Overflow is lost.
   eof marked if EOF encountered.
   if *opt_buf is NULL a new string of size not larger than maxlen-1 is
   allocated and returned in *opt_buf.
 */
int dgetdups(int fd, int *opt_eof, int maxlen, char **opt_buf);
int bugetdups(int fd, int *opt_eof, int maxlen, char **opt_buf);

 /*
   Upto "upto" EOF.
   NULL term.
   "upto" stored in buffer if encountered before maxlen reached.
   Overflow is lost.
   eof marked if EOF encountered.
 */
int dgetupto(int fd, int *opt_eof, int maxlen, int *opt_lost, char *opt_buf, char *upto, int upto_len);
int bugetupto(int fd, int *opt_eof, int maxlen, int *opt_lost, char *buf, char *upto, int upto_len);
int bugetdupupto(int fd, int *opt_eof, int maxlen, int *opt_lost, char **buf, char *upto, int upto_len);

 /*
   Upto LF EOF maxlen.
   NULL term.
   LF stored in buffer if encountered.
   Overflow is left unread.
   eof marked if EOF encountered.
 */
int dreads(int fd, int *opt_eof,int maxlen, char *opt_buf);
int bureads(int fd, int *opt_eof,int maxlen, char *opt_buf);

 /*
   Upto "upto" EOF maxlen.
   NULL term.
   "upto" stored in buffer if encountered.
   Overflow is left unread.
   eof marked if EOF encountered.
 */
int dreadupto(int fd, int *opt_eof, int maxlen, int *opt_complete, char *opt_buf, char *upto, int upto_len);
int bureadupto(int fd, int *opt_eof, int maxlen, int *opt_complete, char *buf, char *upto, int upto_len);

/*
 * nstr input
 */

/* unbuffered: */
size_t nstrread(int fd, char *buf, size_t size);
size_t nstrread_dup(int fd, char **buf);

/**************
 * I/O Polling
 */
int bupoll(struct pollfd *ufds, unsigned int nfds, int timeout);

/**************
 * DNS service
 */

/* returns 1st adr.*/
in_addr_t sysdnsserv4(in_addr_t *opt_res, int opt_maxres, int *opt_count, int *opt_err);
in_addr_t nsshostfwd4(in_addr_t *opt_res, int opt_maxres, char *hostname, int timeout_ms, int *opt_err);
unsigned char *dnsquery4(in_addr_t dnsserv, unsigned char *pkt, int pktsize, int *recvsize, int timeout_ms, int *opt_err);
unsigned char *dnsquery4udp(in_addr_t dnsserv, unsigned char *pkt, int pktsize, int *recvsize, int timeout_ms, int *opt_err);
unsigned char *dnsquery4tcp(in_addr_t dnsserv, unsigned char *pkt, int pktsize, int *recvsize, int timeout_ms, int *opt_err);
in_addr_t *dnsfwd4(in_addr_t *opt_res, int opt_maxres, char *fqdn, in_addr_t dnsserv, int timeout_ms, int *opt_err);
in_addr_t *dnsmx4(in_addr_t *opt_res, int opt_maxres, char *fqdn, in_addr_t dnsserv, int timeout_ms, int *opt_err);

unsigned char *sysdnsserv6(unsigned char **opt_res, int opt_maxres, int *opt_err);
unsigned char *nsshostfwd6(unsigned char **opt_res, int opt_maxres, char *fqdn, int *opt_err);
unsigned char *dnsfwd6(unsigned char **opt_res, int opt_maxres, char *fqdn, unsigned char *dnsserv, int timeout_ms, int *opt_err);
unsigned char *dnsmx6(unsigned char **opt_res, int opt_maxres, char *fqdn, unsigned char *dnsserv, int timeout_ms, int *opt_err);

/********
 * IPC
 *
 */

/* open a tcp session. returns socket fd. */
int tcpopen(const unsigned char *dst, int d_port, const unsigned char *opt_src, int s_port, int domain, int timeout_ms, int *opt_err);
int nsstcpopen(const char *dstname, int d_port, const char *opt_srcname, int s_port, int domain, int timeout_ms, int *opt_err);

/* open a socket for UDP communication */
int udpopen(int domain, int *opt_err);

/* open a socket for listening */
int tcplisten(const unsigned char *src, int domain, int port);
int nsstcplisten(const char *srcname, int domain, int port);

int tcpaccept(int s, int opt_domain, unsigned char *opt_radr, int timeout_ms);
int nsstcpaccept(int s, int opt_domain, char *opt_radr, int radrsize, int timeout_ms);

enum { IOCON_NONE, IOCON_CTRLFD, IOCON_PIPEFD, IOCON_MEM };
struct iocon {
  int type;
  union {
    int pipefd; /* connect pipefd to child */
    int ctrlfd; /* parents reads/writes to/from ctrlfd */
    void *buf;
  } ep;
  size_t size;
  void (*filter)(int readfd, int writefd); /* install this filter function */
};
struct ipc_worker_data {
  pid_t pid;
  void *data;
  size_t size;
  int fd;
};
struct ipc_stat {
  pid_t pid;
  struct ipc_worker_data out, err;
};

struct ipc_stat *ipcexec(const char *fn, struct iocon *opt_in, struct iocon *opt_out, struct iocon *opt_err, char *const *opt_argv, char *const *opt_envp);
int ipcwait(struct ipc_stat *status, int *opt_exit_status);

/*
 * Examples:
 * dnsfwd4(&adr, 1, "dst.net", sysdnsserv4(NULL, 0, &err), -1, &err);
 * tcpopen(&adr, 80, NULL, 0, AF_INET, -1, &err);
 * If function is called with *err != 0 then the function is aborted.
 *
 * nsshostfwd4(&adr, 1, "dst.net", &err);
 * tcpopen( &adr, 80, NULL, 0, AF_INET, -1, &err);
 *
 * ntcpopen( "dst.net", 80, NULL, 0, AF_INET, -1, &err);
 */

/****************
 * Bitstream I/O
 */

struct jelbs {
  int eof;
  char *mem;
  size_t size;
  off_t bytepos;
  int bitpos;
};

struct jelbs *bsopen(char *mem, size_t size);
struct jelbs *bsdup(struct jelbs *s);
int bsclose(struct jelbs *s);
int bseof(struct jelbs *s);

/*
  bsseek returns the resulting offset location as measured in bits from
  the beginning of the stream.
  Arguments and directives as in lseek() but for bits instead of bytes.
*/
off_t bsseek(struct jelbs *s, off_t offset, int whence);

/*
  FMT:
  s# - char *           strings of # bytes, Null terminated.
  o# - unsigned char *  # nr of octets
  b# - unsigned int *   # nr of bits, first bit as bit 31.
  B# - unsigned int *   # nr of bits, bits aligned to lowest part of word
  Returns number fmt specifiers successfully read.
*/
int bsread(struct jelbs *s, const char *fmt, ...);

#endif
