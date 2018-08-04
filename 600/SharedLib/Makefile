#$Id: Makefile,v 1.8 2007/09/14 19:53:05 lizhijie Exp $

ROOT_DIR:=$(shell pwd)
export ROOT_DIR

# 使用新版的STC单片机程序，2007.05.02
# 修改STC单片机为条件编译选项，2007.09.14
STC_NEW=no
# yes
STC_HOME=$(ROOT_DIR)/as600_StcPbx
export STC_NEW


ASSIST_IXP_RELEASES_HOME=/works/ixp_releases
#BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
BUILDTIME := $(shell TZ=CN date -u "+%Y_%m_%d")
ifeq ($(STC_NEW),yes)
	ASSIST_RELEASES_NAME=as600_SharedLib_newStc_$(BUILDTIME).tar.gz  
	ASSIST_RELEASES_NAME_FLASH=as600_SharedLib_flash_newStc_$(BUILDTIME).tar.gz  
else
	ASSIST_RELEASES_NAME=as600_SharedLib_oldStc_$(BUILDTIME).tar.gz  
	ASSIST_RELEASES_NAME_FLASH=as600_SharedLib_flash_oldStc_$(BUILDTIME).tar.gz  
endif 

export BUILDTIME
export ASSIST_RELEASES_NAME
export ASSIST_RELEASES_NAME_FLASH

SUBDIRS += lib
SUBDIRS += programs/recorder
SUBDIRS += programs/renew
SUBDIRS += programs/selftest
SUBDIRS += programs/uartd
SUBDIRS += programs/ipcs
SUBDIRS += tests
SUBDIRS += tests/setup
		     

all: 
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

clean: 
	rm -rf as600_SharedLib_*
	rm -rf pkg
	rm -rf flash_releases
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

cleancdr:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done
	
install:
	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)
	@$(SHELL) $(ROOT_DIR)/installas600shared.sh $(ROOT_DIR)/Linux.bin.arm $(ROOT_DIR)/releases  

FLASH_SUBDIRS += lib

flash:
	for i in $(FLASH_SUBDIRS) ; do ( cd $$i && $(MAKE) all FLASH=YES ) ; done
	@$(SHELL) $(ROOT_DIR)/installas600shared_flash.sh $(ROOT_DIR)/Linux.bin.arm $(ROOT_DIR)/releases  
	
#	rm -rf assist_strace_*
#	cp -R $(ROOT_DIR)/releases/* Linux.bin.arm
#	- find Linux.bin.arm -name \CVS -exec rm -r -f {} \;
#	cd Linux.bin.arm; tar cvzf $(ASSIST_RELEASES_NAME) *; mv $(ASSIST_RELEASES_NAME) ../ 
#	mv $(ASSIST_RELEASES_NAME)  $(ASSIST_IXP_RELEASES_HOME)
#	cd assist && $(MAKE) $@
