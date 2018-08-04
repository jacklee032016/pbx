#$Id: Makefile,v 1.18 2007/09/15 19:45:13 lizhijie Exp $

ROOT_DIR:=$(shell pwd)
export ROOT_DIR

# 使用新版的STC单片机程序，2007.05.02
# 修改STC单片机为条件编译选项，2007.09.14
STC_NEW=no
# yes
STC_HOME=$(ROOT_DIR)/as600_StcPbx
export STC_NEW


ASSIST_IXP_RELEASES_HOME=/works/ixp_releases
# BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d")
ifeq ($(STC_NEW),yes)
	ASSIST_RELEASES_NAME=as600_webadmin_newStc_$(BUILDTIME).tar.gz  
	ASSIST_RELEASES_NAME_FLASH=as600_webadmin_flash_newStc_$(BUILDTIME).tar.gz  
else
	ASSIST_RELEASES_NAME=as600_webadmin_oldStc_$(BUILDTIME).tar.gz  
	ASSIST_RELEASES_NAME_FLASH=as600_webadmin_flash_oldStc_$(BUILDTIME).tar.gz  
endif 

export BUILDTIME
export ASSIST_RELEASES_NAME
export ASSIST_RELEASES_NAME_FLASH

SUBDIRS += lib

# not use this module, lizhijie,2006.11.21
#	modules/restart \

 NEWMODULES += \
	modules/misc \
	modules/network/route \
	modules/system/disk \
	modules/sip/sip \
	modules/network/nat \
	modules/sip/party \
	modules/servers/www \
	modules/servers/mail \
	modules/system/sysInfo \
	modules/system/data \
	modules/quickWizard \
	modules/misc/sysconf \

TESTMODULES += \
	modules/misc/navigate \
	modules/misc/lcd \
	modules/sip/pbx \
	modules/sip/pbx/firmware \
	modules/sip/record \
	modules/sip/alert \
	modules/sip/cdr \
	modules/sip/stc \
	modules/servers/services \
	modules/network/wifi \
	modules/network/video \
	modules/wizard \
	modules/network/scanner \
	modules/ddns/cgi \
	modules/network/vpn \
	modules/ddns 

ifeq ($(STC_NEW),yes)
	TESTMODULES += \
			modules/sip/pbx/hwCheck \
			modules/sip/cgiStc 
else
	TESTMODULES += \
			modules/sip/cgipbx 
endif

#	modules/ddns/test \
#	modules/sip/cgipbx \

ifeq ($(MESH_SUPPORT),YES)
TESTMODULES += modules/network/mesh
endif

SUBDIRS += \
	$(OLDMODULES) \
	$(NEWMODULES) \
	$(TESTMODULES)

# not used
# SUBDIRS += modules/misc/help
# SUBDIRS += modules/cgisource 


FILE_LC=filelist.lc
FILE_PO=webadmin.po
FILE_PO_CN=webadmin_zh_CN.GB2312.po
FILE_PO_JP=webadmin_ja.po

all: 
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

clean: 
	rm -rf as600_webadmin_*
	rm -rf $(VNAME).ver
	rm -rf flash_releases
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

cleancdr:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done


ver:
	@echo Create version info of $(VNAME):
	@if [ -z "$(VFILE)" ]; then echo "No version info found, Skip"; else  \
	echo "$(VNAME) (version: $(V1).$(V2).$(V3)-$(V4) $(VTIME))" > $(VNAME).ver; \
	echo "$(VNAME) (version: $(V1).$(V2).$(V3)-$(V4) $(VTIME))";  \
	fi

# ver
install:
#	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)
#	@$(SHELL) $(ROOT_DIR)/installas600Web.sh $(ROOT_DIR)/Linux.bin.arm $(ROOT_DIR)/releases  
	@$(SHELL) $(ROOT_DIR)/scripting/installas600Web_flash.sh $(ROOT_DIR)/Linux.bin.arm $(ROOT_DIR)/releases  
	rm -rf assist_webadmin*
	rm -rf Linux.bin.arm/var/admin/ja/swat
	rm -rf Linux.bin.arm/var/admin/zh_CN.GB2312/swat
	cp -R $(ROOT_DIR)/releases/* Linux.bin.arm
	- find Linux.bin.arm -name \CVS -exec rm -r -f {} \;
	chmod 755 Linux.bin.arm -R
#	mkdir -p Linux.bin.arm/var/ver
	$(ROOT_DIR)/scripting/buildver.sh $(ROOT_DIR)/Linux.bin.arm
#	$(ROOT_DIR)/scripting/install_link.sh $(ROOT_DIR)/Linux.bin.arm
	cd Linux.bin.arm; tar czf $(ASSIST_RELEASES_NAME) *;mv $(ASSIST_RELEASES_NAME) $(ROOT_DIR)
#	mv $(ASSIST_RELEASES_NAME)  $(ASSIST_IXP_RELEASES_HOME)
#	cd assist && $(MAKE) $@


res:
	@echo Updating Assist Web Locale Data...
#	$(ROOT_DIR)/scripting/buildres.sh $(ROOT_DIR)
	echo Updating Assist Web Locale Data...
	echo "# webadmin source file list" > $(FILE_LC)
	echo "#" >> $(FILE_LC)
	find -name "*.h" -exec ls {} >> $(FILE_LC) \;
	find -name "*.c" -exec ls {} >> $(FILE_LC) \;
	xgettext -f $(FILE_LC) -o $(FILE_PO) -k_  -E --copyright-holder="Assist Software"

	(cat webadmin.header; \
	 tail +6 $(FILE_PO) | sed -e '1,$$s/PACKAGE VERSION/ASWebAdmin-1.2/' \
	     -e '1,$$s/charset=CHARSET/charset=utf-8/'; \
	 cat webadmin.footer) > $(FILE_PO_CN)
	(cat webadmin.header; \
	 tail +6 $(FILE_PO) | sed -e '1,$$s/PACKAGE VERSION/ASWebAdmin-1.2/' \
	     -e '1,$$s/charset=CHARSET/charset=utf-8/'; \
	 cat webadmin.footer) > $(FILE_PO_JP)
	echo Merging changes into Locale... 
	msgmerge -o $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_CN) $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_CN) $(FILE_PO_CN) 
	msgmerge -o $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_JP) $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_JP) $(FILE_PO_JP) 

#	msgmerge -o $(ROOT_DIR)/releases/$(FILE_PO_CN) --no-location $(ROOT_DIR)/releases/$(FILE_PO_CN) $(FILE_PO_CN) 
#	msgmerge -o $(ROOT_DIR)/releases/$(FILE_PO_JP) --no-location $(ROOT_DIR)/releases/$(FILE_PO_JP) $(FILE_PO_JP) 
