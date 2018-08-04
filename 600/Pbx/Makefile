# $Log: Makefile,v $
# Revision 1.2  2007/09/14 20:41:18  lizhijie
# dd
#
# Revision 1.1.1.1  2006/11/29 09:50:50  lizhijie
# AS600 Pbx
#
# Revision 1.2  2006/07/13 05:26:07  lizhijie
# dd
#
# Revision 1.1.1.1  2006/07/12 16:16:47  lizhijie
# rebuild 
#
#
#$Id: Makefile,v 1.2 2007/09/14 20:41:18 lizhijie Exp $

ROOT_DIR:=$(shell pwd)
export ROOT_DIR

# 使用新版的STC单片机程序，2007.05.02
# 修改STC单片机为条件编译选项，2007.09.14
STC_NEW=no
# yes
STC_HOME=$(ROOT_DIR)/as600_StcPbx
export STC_NEW


BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d")

ifeq ($(STC_NEW),yes)
	ASSIST_RELEASES_NAME=as600_Pbx_newStc_$(BUILDTIME).tar.gz 
else
	ASSIST_RELEASES_NAME=as600_Pbx_oldStc_$(BUILDTIME).tar.gz 
endif

export BUILDTIME
export ASSIST_RELEASES_NAME


SUBDIRS += assist 
SUBDIRS += base 
SUBDIRS += state
SUBDIRS += op
SUBDIRS += ua

all: ver
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

clean: 
	rm -rf as600_Pbx_*
	rm -rf pkg
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

cleancdr:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

ver:
	@echo Create version info of $(VNAME):
	@if [ -z "$(VFILE)" ]; then echo "No version info found, Skip"; else  \
	echo "$(VNAME) (version: $(V1).$(V2).$(V3)-$(V4) $(VTIME))" > $(VNAME).ver; \
	echo "$(VNAME) (version: $(V1).$(V2).$(V3)-$(V4) $(VTIME))";  \
	fi

install:
	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)
	@$(SHELL) $(ROOT_DIR)/installas600pbx.sh $(ROOT_DIR)/Linux.bin.arm $(ROOT_DIR)/releases  


#	- find releases -name \CVS -exec rm -r -f {} \;
#	touch assist_pbx_0.tar
#	rm assist_pbx_*.tar
#	mkdir -p releases/usr/bin
#	mkdir -p releases/var/ver
#	install pbx releases/usr/bin
#	install *.ver releases/var/ver
#	install voips/bin/$(ARCH)/*.so releases/lib
#	install shared_lib/libs/bin/$(ARCH)/*.so releases/lib	
#	install Linux.bin.$(ARCH)/*.so releases/lib
#	chmod +x releases/sbin/*
#	tar cvf $(ASSIST_RELEASES_NAME) -C releases  usr lib sbin var
#	mv $(ASSIST_RELEASES_NAME)  $(ASSIST_IXP_RELEASES_HOME)
#	cd assist && $(MAKE) $@
