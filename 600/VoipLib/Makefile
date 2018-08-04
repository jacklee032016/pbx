#$Id: Makefile,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $

ROOT_DIR:=$(shell pwd)
export ROOT_DIR

ASSIST_IXP_RELEASES_HOME=/works/ixp_releases
#BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d")
ASSIST_RELEASES_NAME=as600_VoipLib_$(BUILDTIME).tar.gz 

SUBDIRS += util 
SUBDIRS += rtp 
SUBDIRS += sdp  
SUBDIRS += sipstack  
 
	
all:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done


clean: 
	rm -rf as600_VoipLib_*
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

cleancdr:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done
	
install:
	cd Linux.bin.arm; tar cvzf $(ASSIST_RELEASES_NAME) lib; mv $(ASSIST_RELEASES_NAME) $(ROOT_DIR)
#	- find releases -name \CVS -exec rm -r -f {} \;
#	mkdir -p releases/usr/bin
#	install pbx releases/usr/bin
#	tar cvf $(ASSIST_RELEASES_NAME) -C releases  usr
#	mv $(ASSIST_RELEASES_NAME)  $(ASSIST_IXP_RELEASES_HOME)
#	cd assist && $(MAKE) $@
#	-@cp Linux.bin.arm/*.so releases/lib
#	-@if [ -f releases/usr/bin/test_api ] ; then  tar zcvf $(ASSIST_TEST_API_NAME) -C releases  lib usr ; fi
