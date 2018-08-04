#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
# AS800 SharedLib
#
# Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
# no message
#
# Revision 1.9  2005/11/21 06:44:27  tangr
# add renew and lcdtest in releases
#
# Revision 1.8  2005/09/01 09:47:28  tangr
# fix
#
# Revision 1.7  2005/09/01 08:59:58  tangr
# deal version
#
# Revision 1.6  2005/08/04 03:15:18  lijie
# 1.串口读bug修正
# 2.添加发送dtmf音的接口
#
# Revision 1.5  2005/07/15 07:41:33  wuaiwu
# no message
#
# Revision 1.4  2005/07/08 01:40:31  lijie
# no message
#
# Revision 1.3  2005/06/23 01:46:24  lijie
# *** empty log message ***
#
# Revision 1.2  2005/06/22 01:21:26  lijie
# *** empty log message ***
#
# Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
# add shared library and programs depend on it
#
# $Revision: 1.1.1.1 $
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
#

BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
UARTD_RELEASES_NAME=assist_sharedlib_$(BUILDTIME).tar 

TOP_DIR:=$(shell pwd)

export TOP_DIR

ifeq ($(ARCH),)
ARCH=i686
endif

SUBDIRS = src 

drivers:
	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) all ) ; done

all: drivers

releasedir:
	mkdir -p release/bin.$(TARGET_NAME)
	mkdir -p release/lib.$(TARGET_NAME)


cleanall:
	find . -name 'obj.*' -print -exec /bin/rm -rf {} \;
	find . -name 'bin.*' -print -exec /bin/rm -rf {} \;
	find . -name '*.d' -print -exec /bin/rm -f {} \;
	find . -name '.link_host' -print -exec /bin/rm -f {} \;
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) cleanall ) ; done
	rm -rf release/bin.*/*
	rm -rf release/lib.*/*
	rm -rf release/include/*

veryclean: clean
	- find . -name 'obj.*' -print -exec /bin/rm -rf {} \;
	- find . -name 'bin.*' -print -exec /bin/rm -rf {} \;
	- find . -name '*.o' -print -exec /bin/rm -f {} \;
	- find . -name '*.a' -print -exec /bin/rm -f {} \;
	- find . -name 'core' -print -exec /bin/rm -f {} \;
	- find . -name '.link_host' -print -exec /bin/rm -f {} \;


clean:
	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) clean ) ; done
	- find . -name '*.d' -print -exec /bin/rm -f {} \;
install:
	- find releases -name \CVS -exec rm -r -f {} \;
	touch assist_sharedlib_0.tar
	rm assist_sharedlib*.tar
	mkdir -p releases/usr/bin
	mkdir -p releases/lib
	mkdir -p releases/usr/etc
	mkdir -p releases/usr/pbx/Tone
	mkdir -p releases/var/ver
	install Linux.bin.$(ARCH)/uartd releases/usr/bin
	install Linux.bin.$(ARCH)/recorder releases/usr/bin
	install Linux.bin.$(ARCH)/selftest releases/usr/bin
	install Linux.bin.$(ARCH)/renew releases/usr/bin
	install Linux.bin.$(ARCH)/lcdtest releases/usr/bin
	install src/recorder/files/taper.cfg releases/usr/etc
	install src/recorder/files/sttt.u releases/usr/pbx/Tone
	install src/recorder/files/entt.u releases/usr/pbx/Tone
	install ../libs/bin/$(ARCH)/libaslib.so releases/lib
	install *.ver releases/var/ver
	tar cvf $(UARTD_RELEASES_NAME) -C releases usr/bin usr/etc usr/pbx lib var
