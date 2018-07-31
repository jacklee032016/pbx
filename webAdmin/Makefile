#$Id$

ROOT_DIR:=$(shell pwd)
export ROOT_DIR

include $(ROOT_DIR)/Rules.mak

RELEASES_NAME=webadmin_$(GCC_VERSION)_$(ARCH)_$(EDITION)_$(BUILDTIME).tar.gz

export ARCH
export BUILDTIME
export RELEASES_NAME

SUBDIRS += lib


 NEWMODULES += \
	modules/navigate \
	modules/system/upload \
	modules/system/syscfg \
	modules/system/sysInfo \
	modules/servers/services \
	modules/servers/ddns \
	modules/help \
	modules/system/data \
	modules/wizard \
	modules/network/wifi \
	modules/video \
	modules/autorecordtest 

#	modules/network/route \
#	modules/misc \


TESTMODULES += \
	modules/ddns 
#	modules/quickWizard \


SUBDIRS += \
	$(OLDMODULES) \
	$(NEWMODULES) \

#	$(TESTMODULES)

# not used


FILE_LC=filelist.lc
FILE_PO=webadmin.po
# FILE_PO_CN=webadmin_zh_CN.GB2312.po
# FILE_PO_JP=webadmin_ja.po
FILE_PO_CN=webadmin_zh-cn.po
FILE_PO_TW=webadmin_zh-tw.po

all: 
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

clean: 
	rm -rf webadmin_*
	rm -rf $(VNAME).ver
	- find . -name Linux.bin.$(ARCH) -prune -exec rm -r -f {} \;
	- find . -name Linux.obj.$(ARCH) -prune -exec rm -r -f {} \;

	
#	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done



install:
	rm -rf webadmin*
	cp -R $(ROOT_DIR)/releases/* Linux.bin.$(ARCH)
	- find Linux.bin.$(ARCH) -name \.svn -exec rm -r -f {} \;
	chmod 755 Linux.bin.$(ARCH) -R
#	mkdir -p Linux.bin.$(ARCH)/var/ver
	$(ROOT_DIR)/scripting/buildver.sh $(ROOT_DIR)/Linux.bin.$(ARCH)
	cd Linux.bin.$(ARCH); tar czf $(RELEASES_NAME) *;mv $(RELEASES_NAME) $(ROOT_DIR)


res:
	@echo Updating Web Admin Locale Data...
#	$(ROOT_DIR)/scripting/buildres.sh $(ROOT_DIR)
	echo Updating Web Admin Locale Data...
	echo "# webadmin source file list" > $(FILE_LC)
	echo "#" >> $(FILE_LC)
	find -name "*.h" -exec ls {} >> $(FILE_LC) \;
	find -name "*.c" -exec ls {} >> $(FILE_LC) \;
	xgettext -f $(FILE_LC) -o $(FILE_PO) -k_  -E --copyright-holder="XXX Technology"

	(cat webadmin.header; \
	 tail +6 $(FILE_PO) | sed -e '1,$$s/PACKAGE VERSION/WebAdmin-1.2/' \
	     -e '1,$$s/charset=CHARSET/charset=utf-8/'; \
	 cat webadmin.footer) > $(FILE_PO_CN)
	(cat webadmin.header; \
	 tail +6 $(FILE_PO) | sed -e '1,$$s/PACKAGE VERSION/WebAdmin-1.2/' \
	     -e '1,$$s/charset=CHARSET/charset=utf-8/'; \
	 cat webadmin.footer) > $(FILE_PO_TW)
	echo Merging changes into Locale... 
	msgmerge -o $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_CN) $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_CN) $(FILE_PO_CN) 
	msgmerge -o $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_TW) $(ROOT_DIR)/releases/var/admin/locale/$(FILE_PO_TW) $(FILE_PO_TW) 

#	msgmerge -o $(ROOT_DIR)/releases/$(FILE_PO_CN) --no-location $(ROOT_DIR)/releases/$(FILE_PO_CN) $(FILE_PO_CN) 
#	msgmerge -o $(ROOT_DIR)/releases/$(FILE_PO_JP) --no-location $(ROOT_DIR)/releases/$(FILE_PO_JP) $(FILE_PO_JP) 


package: clean
	cd ..; tar -cvjf webAdmin.$(BUILDTIME).tar.bz2 webAdmin