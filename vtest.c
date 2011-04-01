/*
 * This file is part of "jelio".
 * jelio is an input/output library that replaces the standard C IO library.
 *
 * Copyright: Jens Låås, SLU 2004
 * Copyright license: According to GPL, see file COPYING in this directory.
 *
 */
#include "jeliof.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/wait.h>

void fail(char *str)
{
  printv("ERR: %% failed\n", s(str));
  exit(1);
}

void faild(char *str, int d)
{
  printv("ERR: %% failed with: %%\n", s(str), ld(d));
  exit(1);
}

void faildd(char *str, int d, int d2)
{
  printv("ERR: %% failed with: %%,%%\n", s(str), ld(d),ld(d2));
  exit(1);
}

void fails(char *str, char *s)
{
  printv("ERR: %% failed with: %%\n", s(str), s(s));
  exit(1);
}

void failsd(char *str, char *s, int i)
{
  printv("ERR: %% failed with: %%, %%\n", s(str), d(i), s(s));
  exit(1);
}

int main()
{
  char buf[8];

#if 0  
  dprintv(1, "Hej! %% är %%% gammal!\n", s("Jens"), d(20));
  buprintv(1, "Hej! %% är %%% gammal!\n", s("Jens"), d(20));
  buf[7] = 1;
  sprintv(buf, 7, "12345678");
  dprintv(1, "<%%>\n", s(buf));
  sprintv(buf, 7, "1234%%", s("5678"));
  dprintv(1, "<%%>\n", s(buf));
  sprintv(buf, 7, "1234%%", s("56"));
  dprintv(1, "<%%>\n", s(buf));
  sprintv(buf, 7, "1234%%", s("567"));
  dprintv(1, "<%%>\n", s(buf));
  if(buf[7] != 1) dprintv(1, "1. Corruption! at end byte\n");

  buf[2] ='a';
  buf[3] ='b';
  sprintv(buf, 7, "%%", s("XX"));
  if(buf[2] != 0) dprintv(1, "Corruption! at end byte\n");
  if(buf[3] != 'b') dprintv(1, "Corruption! at end+1 byte\n");


  buprintv(1, "True=%%, False=%%\n", b(1), b(0));

  dprintv(1, "Test printv:\n");
  printv("Testing.\n");
  printv("Testing%%\n", s("."));
  printv("Testing %%\n", s("testing"));
  printv("Testing %%\n", d(1234));
  printv("Testing %% %%\n", s("testing"), d(1234));
  dprintv(1, "Test fprintv:\n");
  fprintv(stdout, "Testing %% %%\n", s("testing"), d(1234));
  printv(" long int size = %%\n", d(sizeof(long int) *8));
  printv(" long long int size = %%\n", d(sizeof(long long int) *8));
  printv("2^30 = 1073741824\n"
	 "     = %%\n" , ld((long int)1 << 30));
  printv("2^62 = 4611686018427387904\n"
	 "     = %%\n" , lld((long long int)1 << 62));
  printv("-2^62 = -4611686018427387904\n"
	 "      = %%\n" , lld(-((long long int)1 << 62)));
  printv("2^31 = 2147483648\n"
	 "     = %%\n", u(1 << 31));
  printv("2^63 = 9223372036854775808\n"
	 "     = %%\n" , llu((unsigned long long int)1 << 63));
  printv("%% = %%\n", t(time(0)), at(time(0)));
  printv(":%%: dF 12,6,4\n", dF(12, 6, 4));
  printv(":%%: dF -12,6,4\n", dF(-12, 6, 4));
  printv(":%%: dF -12,4,4\n", dF(-12, 4, 4));
  printv(":%%: dF -12,4,0\n", dF(-12, 4, 0));
  printv(":%%: dF -12,0,4\n", dF(-12, 0, 4));
  printv(":%%: uF 12,4,4\n", uF(12, 4, 4));

  lprintv("test.log", 0600, "%% %%\n", s("Logging .."), d(123));
  lprintv("test.log", 0600, "%%: %% %%\n", at(time(0)), s("Logging .."), d(345));
  printv("Done!\n");

  printv("\n### Real tests:\n\n");
#endif
  unlink("test.log");

  unlink("test.file");

  {
    char *b2=NULL;
    struct addrinfo *adr = NULL, hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;

    if(getaddrinfo("localhost", NULL, &hints, &adr))
      fail("getaddrinfo");
    if(!adr) fail("getaddrinfo2");
    sprintv_dup(&b2, 1024, "%%", o_sockaddr(adr->ai_addr));
    if(strcmp(b2, "127.0.0.1")) fails("o_sockaddr", b2);
  }
  
  {
    /* sbprintv */
    char *out=NULL;
    size_t ulen, mlen;
    int n;

    if(sbuprintv(&out, &ulen, &mlen, 64, "12345678")!= 8)
      fail("sbuprintv 1");
    if(strcmp(out, "12345678")) fail("sbuprintv 2");
    if(ulen != 8) fail("sbuprintv 3");
    if(mlen != 9) fail("sbuprintv 4 -- default bufsize");
    if(sbuprintv(&out, &ulen, &mlen, 64, "abcd")!= 4)
      fail("sbuprintv 5");
    if(strcmp(out, "12345678abcd")) fail("sbuprintv 6");
    if(ulen != 12) fail("sbuprintv 7");

    out = strdup("HEAD");
    ulen = 4;
    mlen = 5;
    if(sbuprintv(&out, &ulen, &mlen, 64, "ab%%", s("cd"))!= 4)
      fail("sbuprintv 8");
    if(strcmp(out, "HEADabcd")) fail("sbuprintv 9");
    if(ulen != 8) fail("sbuprintv 10");

    out = NULL;
    if(sbuprintv(&out, &ulen, &mlen, 4, "%%", s("12345678"))!= 3)
      fail("sbuprintv 11");
    if( (n=sbuprintv(&out, &ulen, &mlen, 4, "%%", s("12345678"))) != -2)
      faild("sbuprintv 12", n);
    if( (n=sbuprintv(&out, &ulen, &mlen, 4, "%%", s("1"))) != -2)
      faild("sbuprintv 13", n);
    if(strcmp(out, "123")) fail("sbuprintv 14");

    out = NULL;
    if(sbuprintv(&out, &ulen, &mlen, 7, "1234")!= 4)
      fail("sbuprintv 15");
    if(sbuprintv(&out, &ulen, &mlen, 7, "5678")!= 2)
      fail("sbuprintv 16");
    if(sbuprintv(&out, &ulen, &mlen, 7, "5678")!= -2)
      fail("sbuprintv 17");
    if(strcmp(out, "123456")) fail("sbuprintv 18");
  }
  
  /* cprintv */
  if(cprintv("123") != 3) fail("cprintv 1");
  if(cprintv("123%%", s("456")) != 6) fail("cprintv 2");
  if(cprintv("1%%3%%5", s("2"),s("4")) != 5) fail("cprintv 3");
  
  /* sprintv_dup */
  {
    char *dup;
    char *s8 = "12345678";
    if(sprintv_dup(&dup, 256, "1%%3%%5", s("2"),s("4")) != 5) fail("sprintv_dup 1");
    if(strcmp(dup, "12345")) fail("sprintv_dup 2");
    if(sprintv_dup(&dup, 6, "1%%3%%5", s("2"),s("4")) != 5) fail("sprintv_dup 3");
    if(strcmp(dup, "12345")) fail("sprintv_dup 4");
    if(sprintv_dup(&dup, 5, "1%%3%%5", s("2"),s("4")) != 4) fail("sprintv_dup 5");
    if(strcmp(dup, "1234")) fail("sprintv_dup 6");
    if(sprintv_dup(&dup, 4, "1%%", s("234")) != 3) fail("sprintv_dup 7");
    if(strcmp(dup, "123")) fail("sprintv_dup 8");
    if(sprintv_dup(&dup, 4, "1%%", s("23")) != 3) fail("sprintv_dup 9");
    if(strcmp(dup, "123")) fail("sprintv_dup 10");
    if(sprintv_dup(&dup, 256, "x%%", s("123456781234567812345678")) != 25) fail("sprintv_dup 11");
    if(strcmp(dup, "x123456781234567812345678")) fail("sprintv_dup 12");
    if(sprintv_dup(&dup, 256, "x%%%%%%", s(s8), s(s8), s(s8)) != 25) fail("sprintv_dup 13");
    if(strcmp(dup, "x123456781234567812345678")) fail("sprintv_dup 14");
  }

  lprintv("test.log", 0600, "%%.%%\n", s("1ä\xFF"), d(567));
  {
    int fd;
    char b2[16];
    
    fd = open("test.log", O_RDONLY);
    if(dgets(fd, NULL, 16, NULL)!=8) fail("dgets 1");
    close(fd);
    fd = open("test.log", O_RDONLY);
    if(dgets(fd, NULL, 8, buf)!=7) fail("dgets 2");
    if(strcmp(buf, "1ä\xFF.567")) fail("dgets 3");
    close(fd);
    fd = open("test.log", O_RDONLY);
    if(dgets(fd, NULL, 16, b2)!=8) fail("dgets 4");
    if(strcmp(b2, "1ä\xFF.567\n")) fail("dgets 5");
    close(fd);

    fd = open("test.log", O_RDONLY);
    if(bugets(fd, NULL, 16, NULL)!=8) fail("bugets 1");
    close(fd);
    buclose(fd);
    fd = open("test.log", O_RDONLY);
    if(bugets(fd, NULL, 8, buf)!=7) fail("bugets 2");
    if(strcmp(buf, "1ä\xFF.567")) fail("bugets 3");
    close(fd);
    buclose(fd);
    fd = open("test.log", O_RDONLY);
    if(bugets(fd, NULL, 16, b2)!=8) fail("bugets 4");
    if(strcmp(b2, "1ä\xFF.567\n")) fail("bugets 5");
    close(fd);
    buclose(fd);
  }
  unlink("test.log");

  {
    char b2[16];
    int i;
    int v;
    
    strcpy(b2, "he \t j");
    if(sscanv(b2, &i, "he j")!=0) fail("sscanv 1");
    if(i != 6) {
      printv("consumed = %%\n", d(i));
      fail("sscanv 2");
    }
    if(sscanv(b2, &i, "he x")!=0) fail("sscanv 3");
    if(i != 5) fail("sscanv 4");
    if(sscanv(b2, &i, "hex")!=0) fail("sscanv 5");
    if(i != 2) fail("sscanv 6");
    
    strcpy(b2, "1234");
    v=-1;
    if(sscanv(b2, &i, "%%", i_d(&v))!= 1) fail("i_d 1");
    if(v != 1234) fail("i_d 2");
    if(i != 4) fail("i_d 3");

    strcpy(b2, "2147483647"); /* 2^31-1 */
    v=-1;
    if(sscanv(b2, &i, "%%", i_d(&v))!= 1) fail("i_d 1b");
    if(v != (unsigned int)(1<<31)-1) faild("i_d 2b", v);
    if(i != 10) fail("i_d 3b");
    
    strcpy(b2, "  345x");
    v=-1;
    if(sscanv(b2, &i, "%%", i_d(&v))!= 1) fail("i_d 4");
    if(v != 345) fail("i_d 5");
    if(i != 5) fail("i_d 6");

    strcpy(b2, "  -345x");
    v=1;
    if(sscanv(b2, &i, "%%", i_d(&v))!= 1) fail("i_d 7");
    if(v != -345) faild("i_d 8", v);
    if(i != 6) faild("i_d 9", i);
  }

  {
    char b2[16];
    int i;
    unsigned int v;
    
    strcpy(b2, "1234");
    v=-1;
    if(sscanv(b2, &i, "%%", i_u(&v))!= 1) fail("i_u 1");
    if(v != 1234) fail("i_u 2");
    if(i != 4) fail("i_u 3");
    
    strcpy(b2, "  345x");
    v=-1;
    if(sscanv(b2, &i, "%%", i_u(&v))!= 1) fail("i_u 4");
    if(v != 345) fail("i_u 5");
    if(i != 5) fail("i_u 6");

    strcpy(b2, "4294967295");
    v=-1;
    if(sscanv(b2, &i, "%%", i_u(&v))!= 1) fail("i_u 4b");
    if(v != 0xffffffff) fail("i_u 5b");
    if(i != 10) fail("i_u 6b");

    strcpy(b2, "  -345x");
    v=1;
    if(sscanv(b2, &i, "%%", i_u(&v))!= 0) fail("i_u 7");
    if(i != 0) faild("i_u 9", i);

    strcpy(b2, "4462EE82");
    v=1;
    if(sscanv(b2, &i, "%%", i_X(&v))!= 1) fail("i_X 1a");
    if(i != 8) faild("i_X 1b", v);
    if(v != 1147334274) faild("i_X 1c", v);
  }

  {
    char b2[16];
    sprintv(b2, sizeof(b2), "%%", X(0xff));
    if(strcmp(b2, "FF")) fails("o_X 1", b2);
    sprintv(b2, sizeof(b2), "%%", X(0x12345678));
    if(strcmp(b2, "12345678")) fails("o_X 2", b2);
    sprintv(b2, sizeof(b2), "%%", X(-1));
    if(strcmp(b2, "FFFFFFFF")) fails("o_X 3", b2);
    sprintv(b2, sizeof(b2), "%%", X(0));
    if(strcmp(b2, "0")) fails("o_X 4", b2);
    sprintv(b2, sizeof(b2), "%%", X(0xffffffff));
    if(strcmp(b2, "FFFFFFFF")) fails("o_X 5", b2);
  }

  {
    char b2[16];
    sprintv(b2, sizeof(b2), "%%", x(0xff));
    if(strcmp(b2, "ff")) fails("o_x 1", b2);
    sprintv(b2, sizeof(b2), "%%", x(0x12345678));
    if(strcmp(b2, "12345678")) fails("o_x 2", b2);
    sprintv(b2, sizeof(b2), "%%", x(-1));
    if(strcmp(b2, "ffffffff")) fails("o_x 3", b2);
    sprintv(b2, sizeof(b2), "%%", x(0));
    if(strcmp(b2, "0")) fails("o_x 4", b2);
    sprintv(b2, sizeof(b2), "%%", x(0xffffffff));
    if(strcmp(b2, "ffffffff")) fails("o_x 5", b2);
  }

  {
    char b2[16];
    char out[16];
    int i;
    strcpy(b2, "hej");
    if(sscanv(b2, &i, "%%", i_s(out, 8))!= 1) fail("i_s 1");
    if(strcmp(out, "hej")) fail("i_s 2");
    if(i != 3) fail("i_s 3");

    strcpy(b2, "hej ");
    if(sscanv(b2, &i, "%%", i_s(out, 8))!= 1) fail("i_s 4");
    if(strcmp(out, "hej")) fail("i_s 5");
    if(i != 3) fail("i_s 6");

    strcpy(b2, " hej");
    if(sscanv(b2, &i, "%%", i_s(out, 8))!= 1) fail("i_s 7");
    if(strcmp(out, "hej")) fail("i_s 8");
    if(i != 4) fail("i_s 9");

    strcpy(b2, " hej");
    if(sscanv(b2, &i, "%%", i_s(out, 3))!= 1) fail("i_s 10");
    if(strcmp(out, "he")) fail("i_s 11");
    if(i != 3) fail("i_s 12");
  }
  {
    int fd;
    char out[16];
    int v, i;
    lprintv("test.file", 0600, "hej 123x");
    fd = open("test.file", O_RDONLY);
    if(fd<0) fail("test.file");
    if(buscanv(fd, &i, "%%%%", i_s(out, 8), i_d(&v))!= 2) fail("buscanv 1");
    if(i != 7) fail("buscanv 2");
    if(strcmp(out, "hej")) fail("buscanv 3");
    if(v != 123) fail("buscanv 4");
    close(fd);
    buclose(fd);
    unlink("test.file");
  }

  {
    char *av[4];
    char out[16];
    av[0] = "one";
    av[1] = "2";
    av[2] = "three";
    av[3] = "four";
    sprintv(out, 16, "%%", o_sv(3, av, NULL));
    if(strcmp(out, "one2three")) fail("o_sv 1");
    sprintv(out, 16, "%%", o_sv(3, av, "."));
    if(strcmp(out, "one.2.three")) fail("o_sv 2");
    sprintv(out, 16, "%%", o_sv(3, av, "---"));
    if(strcmp(out, "one---2---three")) fail("o_sv 3");

    av[3] = NULL;
    sprintv(out, 16, "%%", o_svnt(av, "---"));
    if(strcmp(out, "one---2---three")) fail("o_svnt 1");
  }

  {
    char out[16];
    sprintv(out, 16, "%%", o_sub("123456", 2, 3));
    if(strcmp(out, "345")) fail("o_sub 1");
    sprintv(out, 16, "%%", o_sub("123456", 4, 3));
    if(strcmp(out, "56")) fail("o_sub 2");
    sprintv(out, 16, "x%%y", o_sub("123456", 6, 3));
    if(strcmp(out, "xy")) fail("o_sub 3");
    sprintv(out, 16, "x%%y", o_sub("123456", 1600, 3));
    if(strcmp(out, "xy")) fail("o_sub 4");
  }

  {
    char out[16];
    sprintv(out, 16, "%%", o_tr("123456", "1", "_"));
    if(strcmp(out, "_23456")) fails("o_tr 1", out);
    sprintv(out, 16, "%%", o_tr("123456", "3", NULL));
    if(strcmp(out, "12456")) fail("o_tr 2");
  }

  {
    char out[16];
    sprintv(out, 16, "%%", o_b64("SmVuc0zl5XM=", 0));
    if(strcmp(out, "JensLåås")) fails("o_b64 1", out);
    sprintv(out, 16, "%%", o_b64(" \nSmV \n uc0zl5XM= ", 0));
    if(strcmp(out, "JensLåås")) fails("o_b64 1b", out);
    sprintv(out, 16, "%%", o_b64("TOXlcw==", 8));
    if(strcmp(out, "Låås")) fail("o_b64 2");
    sprintv(out, 16, "%%", o_b64("amVuc2z2lcw", 8));
    if(strcmp(out, "jenslö")) fail("o_b64 3");
    sprintv(out, 16, "%%", o_b64("", 0));
    if(strcmp(out, "")) fails("o_b64 4", out);
  }

  {
    char out[16];
    sprintv(out, 16, "%%", o_qp("JensL=E5=E5s", 0));
    if(strcmp(out, "JensLåås")) fails("o_qp 1", out);
    sprintv(out, 16, "%%", o_qp("Jens=\r\nL=E5=E5s", 0));
    if(strcmp(out, "JensLåås")) fails("o_qp 1b", out);
    sprintv(out, 16, "%%", o_qp("", 8));
    if(strcmp(out, "")) fail("o_qp 2");
  }

  {
    char out[16];
    sprintv(out, 16, "%%", o_url("JensL%E5%E5s", 0));
    if(strcmp(out, "JensLåås")) fails("o_url 1", out);
    sprintv(out, 16, "%%", o_qp("Jens +L=E5=E5s", 0));
    if(strcmp(out, "Jens +Låås")) fails("o_url 1b", out);
    sprintv(out, 16, "%%", o_url("", 8));
    if(strcmp(out, "")) fail("o_url 2");
  }
  
  {
    char out[16];
    int consumed;
    unsigned char data[4] = "\001\002 ";
    sprintv(out, 16, "%%", o_xs(data, 3));
    if(strcmp(out, "010220")) fails("o_xs 1", out);
    sprintv(out, 16, "%%", o_xs(data, 4));
    if(strcmp(out, "01022000")) fail("o_xs 2");

    strcpy(out, "2a");
    if(sscanv(out, &consumed, "%%", i_xs(data, 4)) != 1)
      fail("i_xs 0");
    if(consumed != 2) faild("i_xs 0b", consumed);
    data[1] = 0;
    if(strcmp((char*)data, "*")) faild("i_xs 0c", data[0]);

    strcpy(out, "68656a00");
    if(sscanv(out, &consumed, "%%", i_xs(data, 4)) != 1)
      fail("i_xs 1");
    if(consumed != 8) faild("i_xs 1b", consumed);
    if(strcmp((char*)data, "hej")) fails("i_xs 2", (char*)data);
    
    strcpy(out, "010220");
    if(sscanv(out, &consumed, "%%", i_xs(data, 4)) != 1)
      fail("i_xs 3");
    if(consumed != 6) faild("i_xs 3b", consumed);
    data[3] = 0;
    if(strcmp((char*)data, "\001\002 ")) fail("i_xs 4");
    
  }

  {
    int fd;
    char *b2=NULL;
    
    lprintv("test.file", 0600, "12\xFF\n");
    fd = open("test.file", O_RDONLY);
    if(bugetdups(fd, NULL, 16, &b2)!=4) fail("bugetdups 1");
    if(strcmp(b2, "12\xFF\n")) fail("bugetdups 2");
    buclose(fd);
    close(fd);

    unlink("test.file");
    lprintv("test.file", 0600, "123456789012345678");
    lprintv("test.file", 0600, "1234567890123456\xFF\n");
    fd = open("test.file", O_RDONLY);
    b2=NULL;
    if(bugetdups(fd, NULL, 64, &b2)!=36) fail("bugetdups 3");
    if(strcmp(b2, "1234567890123456781234567890123456\xFF\n")) fail("bugetdups 4");
    buclose(fd);
    close(fd);
    
    unlink("test.file");
  }

  lprintv("test.log", 0600, "%%[1mBold%%[22mNot\n", c(27), c(27));

  {
    struct jelbs *s;
    char b[4], b2[4];
    unsigned int t;
    b[0] = 128;
    b[1] = b[2] = b[3] = 0;
    s = bsopen(b, 4);
    if(bsread(s, "B1", &t)!=1) fail("bsread 1a");
    if(t != 1)  faild("bsread 1b", t);
    bsclose(s);
    b[0] = 11;
    s = bsopen(b, 4);
    if(bsread(s, "B8", &t)!=1) fail("bsread 2a");
    if(t != 11)  faild("bsread 2b", t);
    bsclose(s);
    b[0] = 128+11;
    s = bsopen(b, 4);
    if(bsread(s, "B4", &t)!=1) fail("bsread 3a");
    if(t != 8)  faild("bsread 3b", t);
    if(bsread(s, "B2", &t)!=1) fail("bsread 3c");
    if(t != 2)  faild("bsread 3d", t);
    if(bsread(s, "B2", &t)!=1) fail("bsread 3e");
    if(t != 3)  faild("bsread 3f", t);
    bsclose(s);
    b[3] = 1;

    s = bsopen(b, 4);
    if(bsread(s, "B32", &t)!=1) fail("bsread 4a");
    if(t != ((128+11)<<24) + 1)  faild("bsread 4b", t);
    bsclose(s);

    strcpy(b, "hej");
    s = bsopen(b, 4);
    if(bsread(s, "s3", b2)!=1) fail("bsread 5a");
    if(strcmp(b2,b)) fails("bsread 5b", b2);
    bsclose(s);
  }

  {
    char buf[8], *buf2=NULL;
    if(xprintv("T%%!\n", d(123))!= 6) fail("xprintv 1");
    xprintv_push_mem(NULL, buf, 8);
    if(xprintv("T%%!\n", d(123))!= 6) fail("xprintv 2");
    if(strcmp(buf, "T123!\n")) fail("xprintv 3");
    xprintv_push_dmem(NULL, &buf2, 8);
    if(xprintv("T%%!\n", d(123))!= 6) fail("xprintv 4");
    if(strcmp(buf2, "T123!\n")) fail("xprintv 5");
    if(xprintv(".")!= 1) fail("xprintv 6");
    if(strcmp(buf2, "T123!\n.")) fail("xprintv 7");
  }

  {
    int fd, i, complete;
    char out[16];
    lprintv("test.file", 0600, "hej 123xap123groda");
    fd = open("test.file", O_RDONLY);
    if(fd<0) fail("test.file");

    if(bureadupto(fd, NULL, 16, NULL, out, "123", 0)!=7) fail("bureadupto 1");
    if(strcmp(out, "hej 123")) fails("bureadupto 1b", out);
    if( (i=bureadupto(fd, NULL, 6, &complete, out, "123", 0))!=3) fails("bureadupto 2", out);
    if(complete) fail("bureadupto 2b");
    if(strcmp(out, "xap")) fails("bureadupto 3", out);
    if( (i=bureadupto(fd, NULL, 16, &complete, out, "123", 0))!=3) failsd("bureadupto 4", out, i);
    if(!complete) fail("bureadupto 4b");
    
    close(fd);
    buclose(fd);
    unlink("test.file");
  }

  {
    int fd, i, lost;
    char out[16];
    lprintv("test.file", 0600, "hej 123xap123groda");
    fd = open("test.file", O_RDONLY);
    if(fd<0) fail("test.file");

    if(bugetupto(fd, NULL, 16, &lost, out, "123", 0)!=7) fail("bugetupto 1");
    if(strcmp(out, "hej 123")) fails("bugetupto 1b", out);
    if(lost) fails("bugetupto 1c", out);
    
    if( (i=bugetupto(fd, NULL, 6, &lost, out, "123", 0))!=5) fails("bugetupto 2", out);
    if(strcmp(out, "xap12")) fails("bugetupto 2b", out);
    if(lost!=1) faild("bugetupto 2c", lost);

    close(fd);
    buclose(fd);
    unlink("test.file");
  }

  {
    int fd, i, lost;
    char *out=NULL;
    lprintv("test.file", 0600, "hej 123xap123groda");
    fd = open("test.file", O_RDONLY);
    if(fd<0) fail("test.file");

    if(bugetdupupto(fd, NULL, 16, &lost, &out, "123", 0)!=7) fail("bugetupto 1");
    if(strcmp(out, "hej 123")) fails("bugetdupupto 1b", out);
    if(lost) fails("bugetdupupto 1c", out);
    out=NULL;
    if( (i=bugetdupupto(fd, NULL, 6, &lost, &out, "123", 0))!=5) fails("bugetdupupto 2", out);
    if(strcmp(out, "xap12")) fails("bugetdupupto 2b", out);
    if(lost!=1) faild("bugetdupupto 2c", lost);

    close(fd);
    buclose(fd);
    unlink("test.file");
  }

  /* IPC */
  {
    int l, fd ,r, err=0;
    char *s = "/tmp/.localunix_test";

    unlink(s);
    l = tcplisten((unsigned char*)s, AF_UNIX, 0);
    if(l < 0) fails("tcplisten UN 1", s);
    
    fd = tcpopen((unsigned char*)s, 0, NULL, 0, AF_UNIX, -1, &err);
    if(fd < 0) fails("tcpopen UN 1", s);
    close(fd);

    fd = tcpopen((unsigned char*)s, 0, NULL, 0, AF_UNIX, 2000, &err);
    if(fd < 0) fails("tcpopen UN 2", s);
    
    r = tcpaccept(l, 0, NULL, -1);
    if(r < 0) fail("tcpaccept UN");
    close(r);
    close(fd);
    close(l);
    unlink(s);
  }

  {
    int l, fd ,r, err=0;
    int port = 27919;
    in_addr_t dst = htonl(0x7F000001);
    
    //buprintv(2, "dst = %%\n", x(dst));

    l = tcplisten(NULL, AF_INET, port);
    if(l < 0) faild("tcplisten INET 1", port);
    fd = tcpopen((unsigned char *)&dst, port, NULL, 0, AF_INET, -1, &err);
    if(fd < 0) faild("tcpopen INET 1", port);
    close(fd);
    
    fd = tcpopen((unsigned char *)&dst, port, NULL, 0, AF_INET, 2000, &err);
    if(fd < 0) faild("tcpopen INET 2", port);
    close(fd);

    r = tcpaccept(l, AF_INET, NULL, -1);
    if(r < 0) fail("tcpaccept");
    close(r);
    close(fd);
    close(l);
  }

  {
    struct iocon out;
    int rc, status;
    struct ipc_stat *i;
    char *args[3];
    out.type = IOCON_MEM;
    out.ep.buf = malloc(10);
    out.size = 10;
    args[0] = "/bin/echo";
    args[1] = "apa";
    args[2] = NULL;
    if( (i = ipcexec("/bin/echo", NULL, &out, NULL, args, NULL))==0)
      fail("ipcexec 0");
    rc = ipcwait(i, &status);
    if(!WIFEXITED(status))
      faild("ipcexec 0b", status);
    if(WEXITSTATUS(status)!=0)
      faild("ipcexec 0c", WEXITSTATUS(status));
    if(i->out.size != 4) faild("ipcexec 1", out.size);
    if(strncmp(out.ep.buf, "apa\n", 4))
       fail("ipcexec 2");

    buprintv(1, "-- ipcexec test 1 done!\n");
  }

  {
    struct iocon in, out;
    int rc, status;
    struct ipc_stat *i;
    char *args[3];
    in.type = IOCON_MEM;
    in.ep.buf = "Fisk";
    in.size = 4;
    out.type = IOCON_MEM;
    out.ep.buf = malloc(10);
    out.size = 10;
    args[0] = "/bin/cat";
    args[1] = "-";
    args[2] = NULL;
    if( (i = ipcexec("/bin/cat", &in, &out, NULL, args, NULL))==0)
      fail("ipcexec mem-mem 0");
    rc = ipcwait(i, &status);
    if(!WIFEXITED(status))
      faild("ipcexec mem-mem 0b", status);
    if(WEXITSTATUS(status)!=0)
      faild("ipcexec mem-mem 0c", WEXITSTATUS(status));
    if(i->out.size != 4) faild("ipcexec mem-mem 1", out.size);
    if(strncmp(out.ep.buf, "Fisk", 4))
       fail("ipcexec mem-mem 2");
    buprintv(1, "-- ipcexec test 2 done!\n");
  }

  {
    struct iocon in;
    int status, rc;
    struct ipc_stat *i;
    char *args[3];
    in.type = IOCON_MEM;
    in.ep.buf = "Fisk";
    in.size = 4;
    args[0] = "/bin/cat";
    args[1] = "-";
    args[2] = NULL;
    if( (i = ipcexec("/bin/cat", &in, NULL, NULL, args, NULL))==0)
      fail("ipcexec mem-none 0");
    rc = ipcwait(i, &status);
    if(!WIFEXITED(status))
      faild("ipcexec mem-none 0b", status);
    if(WEXITSTATUS(status)!=0)
      faild("ipcexec mem-none 0c", WEXITSTATUS(status));
    buprintv(1, "-- ipcexec test 3 done!\n");
  }

  {
    struct iocon in;
    int status, rc;
    char *args[3];
    struct ipc_stat *i;
    in.type = IOCON_MEM;
    in.ep.buf = "Fisk";
    in.size = 4;
    args[0] = "/bin/sleep";
    args[1] = "2";
    args[2] = NULL;
    if( (i = ipcexec("/bin/sleep", &in, NULL, NULL, args, NULL))==0)
      fail("ipcexec mem-none S0");
    rc = ipcwait(i, &status);
    if(!WIFEXITED(status))
      faild("ipcexec mem-none S0b", status);
    if(WEXITSTATUS(status)!=0)
      faild("ipcexec mem-none S0c", WEXITSTATUS(status));
    buprintv(1, "-- ipcexec test 4 done!\n");
  }

  {
    struct iocon in2,out1,out2;
    int rc, status;
    struct ipc_stat *i, *i2;
    char *args[3];
    out1.type = IOCON_CTRLFD;
    args[0] = "/bin/echo";
    args[1] = "apa";
    args[2] = NULL;
    if( (i = ipcexec("/bin/echo", NULL, &out1, NULL, args, NULL))==0)
      fail("ipcexec ctrlfd-pipefd-mem 0");
    in2.type = IOCON_PIPEFD;
    in2.ep.pipefd = out1.ep.ctrlfd;
    out2.type = IOCON_MEM;
    out2.ep.buf = malloc(10);
    memset(out2.ep.buf, 0, 10);
    out2.size = 10;
    args[0] = "/bin/cat";
    args[1] = "-";
    args[2] = NULL;
    if( (i2 = ipcexec("/bin/cat", &in2, &out2, NULL, args, NULL))==0)
      fail("ipcexec ctrlfd-pipefd-mem 1");
    rc = ipcwait(i, &status);
    rc = ipcwait(i2, &status);
    if(!WIFEXITED(status))
      faild("ipcexec ctrlfd-pipefd-mem 1b", status);
    if(WEXITSTATUS(status)!=0)
      faild("ipcexec ctrlfd-pipefd-mem 1c", WEXITSTATUS(status));
    if(i2->out.size != 4) faild("ipcexec ctrlfd-pipefd-mem 2", out2.size);
    if(strncmp(out2.ep.buf, "apa\n", 4))
       fail("ipcexec ctrlfd-pipefd-mem 3");
    if(status != 0) faild("ipcexec ctrlfd-pipefd-mem 5", rc);
    buprintv(1, "-- ipcexec test 5 done!\n");
  }

  xxprintv_push_null(_X_ERROR);
  xx_error("If this text is visible then an error has occured!\n");
  xx_debug("If this text is visible then an error has occured!\n");

  xx_app("All tests successfull!\n");
  unlink("test.log");
  exit(0);
}
