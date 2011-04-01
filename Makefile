#%switch --prefix PREFIX
#%ifnswitch --prefix /usr PREFIX
#%ifnswitch --mandir $(PREFIX)/share/man MANDIR
#%ifswitch --diet diet DIET
#%setifdef DIET -nostdinc DIETINC
#%seteval X86 uname -m|grep i.86
#%setifdef X86 -march=i386 ARCH
#%seteval SUNOS uname -s|grep SunOS
#%setifdef SUNOS -lsocket -lnsl SOLARIS
#?CC=$(DIET) gcc
#?CFLAGS=-g -Wall -Os $(ARCH) $(DIETINC)
#?PLIBOBJ=jelio_write.o jelio_read.o cprintv.o dprintv.o buprintv.o sprintv.o sprintv_dup.o sbuprintv.o lprintv.o fprintv.o printv.o o_c.o o_s.o o_fs.o o_xs.o o_b64.o o_qp.o o_url.o o_nstr.o o_sub.o o_tr.o o_sv.o o_svnt.o o_d.o o_ld.o o_lld.o o_dF.o o_x.o o_X.o o_u.o o_lu.o o_llu.o o_uF.o o_b.o o_t.o o_uid.o o_gid.o o_mode.o o_pid.o o_at.o o_sockaddr.o o_ip4.o jelio_input_free.o buclose.o dgets.o bugets.o bugetdups.o bugetdupln.o vafprintv.o buscanv.o sscanv.o i_d.o i_ld.o i_u.o i_X.o i_lX.o i_t.o i_pid.o i_gid.o i_uid.o i_lu.o i_xs.o i_Xs.o i_s.o i_set.o i_nstrdup.o i_sdup.o nstrread.o nstrread_dup.o bsopen.o bsclose.o bsdup.o bseof.o bsseek.o bsread.o vabuprintv.o vasbuprintv.o xxprintv.o xprintv.o xxprintv_push_null.o xxprintv_push_fd.o xprintv_push_fd.o xprintv_push_mem.o xprintv_push_dmem.o bureadupto.o bugetupto.o bugetdupupto.o bugetc.o buseek.o bupeekc.o bugetln.o dgetln.o bupoll.o buread.o tcpopen.o tcplisten.o tcpaccept.o udpopen.o dnsfwd4.o sysdnsserv4.o dnsquery4.o dnsquery4udp.o dnsquery4tcp.o ipcexec.o ipcwait.o
#?ILIBOBJ=char_isspace.o jelio_itostr.o jelio_litostr.o jelio_llitostr.o jelio_utostr.o jelio_lutostr.o jelio_llutostr.o jelio_itostrf.o jelio_utostrf.o vascanv.o jelio_buffer.o jelio_sbuffer.o jelio_sbuffer_close.o jelio_buffer_getc.o jelio_buffer_ungetc.o jelio_buffer_peekc.o jelio_buffer_seek.o
#?LIBOBJ=$(PLIBOBJ) $(ILIBOBJ)
#?all:	vtest libjelio.a manpages
#?vtest:	vtest.o $(LIBOBJ)
#?	$(CC) $(CFLAGS) -g -o vtest vtest.o $(LIBOBJ) $(SOLARIS)
#?memleak:	memleak.o $(LIBOBJ)
#?	$(CC) $(CFLAGS) -g -o memleak memleak.o $(LIBOBJ) $(SOLARIS)
#?dnstest:	dnstest.o $(LIBOBJ)
#?	$(CC) $(CFLAGS) -g -o dnstest dnstest.o $(LIBOBJ) $(SOLARIS)
#?libjelio.a:	$(LIBOBJ)
#?	ar cr libjelio.a $(LIBOBJ)
#?clean:	
#?	rm -f *.o vtest memleak dnstest libjelio.a man/*.3 doc/*.html manpages
#?manpages:
#?	mkdir -p doc man
#?	./mk-manpage.sh
#?	./mk-htmlpage.sh
#?	touch manpages
#?manpages-install:	manpages
#?	mkdir -p $(DESTDIR)/$(MANDIR)/man3
#?	cp man/*.3 $(DESTDIR)/$(MANDIR)/man3
#?install:	libjelio.a manpages-install
#?	mkdir -p $(DESTDIR)/$(PREFIX)/lib $(DESTDIR)/$(PREFIX)/include $(DESTDIR)/$(MANDIR)/man3
#?	cp -f libjelio.a $(DESTDIR)/$(PREFIX)/lib
#?	cp -f jelio.h jeliof.h $(DESTDIR)/$(PREFIX)/include
#?tarball:	clean
#?	make-tarball.sh
PREFIX= /usr
MANDIR= $(PREFIX)/share/man
CC=$(DIET) gcc
CFLAGS=-g -Wall -Os $(ARCH) $(DIETINC)
PLIBOBJ=jelio_write.o jelio_read.o cprintv.o dprintv.o buprintv.o sprintv.o sprintv_dup.o sbuprintv.o lprintv.o fprintv.o printv.o o_c.o o_s.o o_fs.o o_xs.o o_b64.o o_qp.o o_url.o o_nstr.o o_sub.o o_tr.o o_sv.o o_svnt.o o_d.o o_ld.o o_lld.o o_dF.o o_x.o o_X.o o_u.o o_lu.o o_llu.o o_uF.o o_b.o o_t.o o_uid.o o_gid.o o_mode.o o_pid.o o_at.o o_sockaddr.o o_ip4.o jelio_input_free.o buclose.o dgets.o bugets.o bugetdups.o bugetdupln.o vafprintv.o buscanv.o sscanv.o i_d.o i_ld.o i_u.o i_X.o i_lX.o i_t.o i_pid.o i_gid.o i_uid.o i_lu.o i_xs.o i_Xs.o i_s.o i_set.o i_nstrdup.o i_sdup.o nstrread.o nstrread_dup.o bsopen.o bsclose.o bsdup.o bseof.o bsseek.o bsread.o vabuprintv.o vasbuprintv.o xxprintv.o xprintv.o xxprintv_push_null.o xxprintv_push_fd.o xprintv_push_fd.o xprintv_push_mem.o xprintv_push_dmem.o bureadupto.o bugetupto.o bugetdupupto.o bugetc.o buseek.o bupeekc.o bugetln.o dgetln.o bupoll.o buread.o tcpopen.o tcplisten.o tcpaccept.o udpopen.o dnsfwd4.o sysdnsserv4.o dnsquery4.o dnsquery4udp.o dnsquery4tcp.o ipcexec.o ipcwait.o
ILIBOBJ=char_isspace.o jelio_itostr.o jelio_litostr.o jelio_llitostr.o jelio_utostr.o jelio_lutostr.o jelio_llutostr.o jelio_itostrf.o jelio_utostrf.o vascanv.o jelio_buffer.o jelio_sbuffer.o jelio_sbuffer_close.o jelio_buffer_getc.o jelio_buffer_ungetc.o jelio_buffer_peekc.o jelio_buffer_seek.o
LIBOBJ=$(PLIBOBJ) $(ILIBOBJ)
all:	vtest libjelio.a manpages
vtest:	vtest.o $(LIBOBJ)
	$(CC) $(CFLAGS) -g -o vtest vtest.o $(LIBOBJ) $(SOLARIS)
memleak:	memleak.o $(LIBOBJ)
	$(CC) $(CFLAGS) -g -o memleak memleak.o $(LIBOBJ) $(SOLARIS)
dnstest:	dnstest.o $(LIBOBJ)
	$(CC) $(CFLAGS) -g -o dnstest dnstest.o $(LIBOBJ) $(SOLARIS)
libjelio.a:	$(LIBOBJ)
	ar cr libjelio.a $(LIBOBJ)
clean:	
	rm -f *.o vtest memleak dnstest libjelio.a man/*.3 doc/*.html manpages
manpages:
	mkdir -p doc man
	./mk-manpage.sh
	./mk-htmlpage.sh
	touch manpages
manpages-install:	manpages
	mkdir -p $(DESTDIR)/$(MANDIR)/man3
	cp man/*.3 $(DESTDIR)/$(MANDIR)/man3
install:	libjelio.a manpages-install
	mkdir -p $(DESTDIR)/$(PREFIX)/lib $(DESTDIR)/$(PREFIX)/include $(DESTDIR)/$(MANDIR)/man3
	cp -f libjelio.a $(DESTDIR)/$(PREFIX)/lib
	cp -f jelio.h jeliof.h $(DESTDIR)/$(PREFIX)/include
tarball:	clean
	make-tarball.sh
