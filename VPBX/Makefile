#$Id: Makefile,v 1.1.1.1 2006/11/30 16:26:28 lizhijie Exp $

ROOT_DIR:=$(shell pwd)
export ROOT_DIR

ASSIST_IXP_RELEASES_HOME=/works/ixp_releases
BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
ASSIST_RELEASES_NAME=assist_vpbx_$(BUILDTIME).tar 
ASSIST_TEST_API_NAME=assist_test_api_$(BUILDTIME).tar.gz 

 SUBDIRS += pbx 
 SUBDIRS += plugins/console  
 SUBDIRS += plugins/pstn 
 SUBDIRS += plugins/voip
 SUBDIRS += plugins/isdn 

ifeq ($(test),api)
	SUBDIRS += test
endif

	
all:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

clean: 
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

cleancdr:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done
	
install:
	- find releases -name \CVS -exec rm -r -f {} \;
#	mkdir -p releases/usr/bin
#	install pbx releases/usr/bin
#	tar cvf $(ASSIST_RELEASES_NAME) -C releases  usr
#	mv $(ASSIST_RELEASES_NAME)  $(ASSIST_IXP_RELEASES_HOME)
#	cd assist && $(MAKE) $@
	-@cp Linux.bin.arm/*.so releases/lib
	-@if [ -f releases/usr/bin/test_api ] ; then  tar zcvf $(ASSIST_TEST_API_NAME) -C releases  lib usr ; fi
