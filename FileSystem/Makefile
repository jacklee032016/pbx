#########################################################################
# �ؽ��ļ�ϵͳ����ʹ�õ�Makefile 
# ��־�� 2006.07.07 
###########################################################################
ROOT_DIR:=$(shell pwd)
export ROOT_DIR

PLATFORM=AS600
#PLATFORM=AS800
export PLATFORM

ASSIST_IXP_RELEASES_HOME=/works/ixp_releases
#BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d")
ASSIST_RELEASES_NAME=$(PLATFORM)_FileSystem_$(BUILDTIME).tar.gz  
ASSIST_RELEASES_NAME_PART_A=$(PLATFORM)_FileSystem_$(BUILDTIME)_PART_A.tar.gz  
ASSIST_RELEASES_NAME_PART_B=$(PLATFORM)_FileSystem_$(BUILDTIME)_PART_B.tar.gz  
ASSIST_RELEASES_NAME_PART_C=$(PLATFORM)_FileSystem_$(BUILDTIME)_PART_C.tar.gz  
ASSIST_RELEASES_NAME_PART_D=$(PLATFORM)_FileSystem_$(BUILDTIME)_PART_D.tar.gz  

export BUILDTIME 
export ASSIST_RELEASES_NAME
export ASSIST_RELEASES_NAME_PART_A
export ASSIST_RELEASES_NAME_PART_B
export ASSIST_RELEASES_NAME_PART_C
export ASSIST_RELEASES_NAME_PART_D

FLASH_IMAGE_SIZE=2424832
# 4M flash, 32K/Block, 32768 bytes/block , total 128 blocks
FLASH_ERASE_SIZE=32768
FLASH_OUTPUT_FILE=$(PLATFORM)_flash_$(BUILDTIME).image

export FLASH_IMAGE_SIZE
export FLASH_ERASE_SIZE
export FLASH_OUTPUT_FILE


# �����Ĵ�����Ŀ��ƽ̨�ϵ�·������Ҫд�����Ĵ�����
DESTDIR:=$(ROOT_DIR)/releases
SRCDIR:=$(ROOT_DIR)/PreReleases
PKGDIR:=$(ROOT_DIR)/Packages

FLASH_DEST_DIR:=$(ROOT_DIR)/flashReleases
FLASH_SRC_DIR:=$(ROOT_DIR)/PreReleases/flash
FLASH_PKG_DIR:=$(ROOT_DIR)/Packages/flash


BUILDDIR:=$(ROOT_DIR)/build
BACKUP_DIR:=$(ROOT_DIR)/backup_$(BUILDTIME)

exec_prefix=${prefix}


all:install
#	-@$(SHELL) $(ROOT_DIR)/scripts/installtar.sh  $(DESTDIR); 
	
# �����CVS��ȡ�õ�Դ��ģ��
build:checkout
	-@$(SHELL) $(ROOT_DIR)/scripts/CommonModulesBuild.sh $(BUILDDIR)/sources/common $(BUILDDIR)/pkg/common;
	-@$(SHELL) $(ROOT_DIR)/scripts/As600ModulesBuild.sh $(BUILDDIR)/sources/as600 $(BUILDDIR)/pkg/as600;

# ����CVS�ĳ��򵽹�����
backup:
	-@$(SHELL) $(ROOT_DIR)/scripts/As600ModulesCheckout.sh $(BACKUP_DIR)/as600;
	-@$(SHELL) $(ROOT_DIR)/scripts/As800ModulesCheckout.sh $(BACKUP_DIR)/as800;
	-@$(SHELL) $(ROOT_DIR)/scripts/CommonModulesCheckout.sh $(BACKUP_DIR)/common;
	-@$(SHELL) $(ROOT_DIR)/scripts/sipServerModulesCheckout.sh $(BACKUP_DIR)/SipServer;
	-@$(SHELL) $(ROOT_DIR)/scripts/macModulesCheckout.sh $(BACKUP_DIR)/mac;
	-@$(SHELL) $(ROOT_DIR)/scripts/s3cModulesCheckout.sh $(BACKUP_DIR)/s3c;
	-@$(SHELL) $(ROOT_DIR)/scripts/testModulesCheckout.sh $(BACKUP_DIR)/test;
	-@$(SHELL) $(ROOT_DIR)/scripts/miscModulesCheckout.sh $(BACKUP_DIR)/misc;
	-@$(SHELL) $(ROOT_DIR)/scripts/wmnModulesCheckout.sh $(BACKUP_DIR)/wmn;
	mkisofs -o /myWorks/cvs_backup_$(BUILDTIME).iso -jcharset cp850 -max-iso9660-filenames -allow-leading-dots -J -r -v -V $(BUILDTIME) /myWorks/fs /CVS/CVS/ 



# ��CVS��ȡ��Դ��ģ��
checkout:
	-@$(SHELL) $(ROOT_DIR)/scripts/CommonModulesCheckout.sh $(BUILDDIR)/sources/common;
	-@$(SHELL) $(ROOT_DIR)/scripts/As600ModulesCheckout.sh $(BUILDDIR)/sources/as600;
		
	
install: installdirs installsrc installpkgs flash

# �����豸Ŀ¼���ļ�ϵͳ������Ҫ��Ŀ¼�ṹ
installdirs:
	-if  [ -d  $(DESTDIR) ]; then rm -rf $(DESTDIR); fi  
	-@$(SHELL) $(ROOT_DIR)/scripts/installdirs.sh $(DESTDIR) 
#	-@$(SHELL) $(ROOT_DIR)/scripts/installdirs.sh $(DESTDIR)/as800 


# ��װ���ȷ�����Ŀ¼�����е��ļ�
installsrc: installdirs
	@$(SHELL) $(ROOT_DIR)/scripts/installsrc.sh $(DESTDIR) $(SRCDIR) 
#	@$(SHELL) $(ROOT_DIR)/scripts/installsrc.sh $(DESTDIR)/as800 $(SRCDIR) 


installpkgs: installdirs installsrc
	@$(SHELL) $(ROOT_DIR)/scripts/installpkgs.sh $(DESTDIR) $(PKGDIR)


# all 
installbin: installdirs
	@$(SHELL) $(srcdir)/script/installbin.sh $(INSTALLPERMS) $(DESTDIR) $(DESTDIR)$(BASEDIR) $(DESTDIR)$(SBINDIR) $(DESTDIR)$(LIBDIR) $(DESTDIR)$(VARDIR) $(SBIN_PROGS)
	@$(SHELL) $(srcdir)/script/install_swat_bin.sh $(INSTALLPERMS) $(DESTDIR)$(SBINDIR) $(DESTDIR)/var/www/cgi-bin swat $(DESTDIR)
	@$(SHELL) $(srcdir)/script/installbin.sh $(INSTALLPERMS) $(DESTDIR) $(DESTDIR)$(BASEDIR) $(DESTDIR)$(BINDIR) $(DESTDIR)$(LIBDIR) $(DESTDIR)$(VARDIR) $(BIN_PROGS)

######################################################
# File System for the 3rd partition on Flash
######################################################
flash:installFlashDir installFlashSrc installFlashPkgs 


installFlashDir:
	-if  [ -d  $(FLASH_DEST_DIR) ]; then rm -rf $(FLASH_DEST_DIR); fi  
	-@$(SHELL) $(ROOT_DIR)/scripts/flash/installFlashDirs.sh $(FLASH_DEST_DIR) 

installFlashSrc: installFlashDir
	@$(SHELL) $(ROOT_DIR)/scripts/flash/installFlashSrc.sh $(FLASH_DEST_DIR) $(FLASH_SRC_DIR) 


installFlashPkgs: installFlashDir installFlashSrc
	@$(SHELL) $(ROOT_DIR)/scripts/flash/installFlashPkgs.sh $(FLASH_DEST_DIR) $(FLASH_PKG_DIR)


clean:
	rm -rf $(DESTDIR)
	rm -rf $(BUILDDIR)
	rm -rf $(FLASH_DEST_DIR)
	rm -rf parts
	rm -rf $(PLATFORM)_FileSystem_*
	rm -rf $(PLATFORM)_flash_*

showlayout:
	@echo "Samba will be installed into:"
	@echo "  basedir:     $(BASEDIR)"
	@echo "  bindir:      $(BINDIR)"
	@echo "  sbindir:     $(SBINDIR)"
	@echo "  libdir:      $(LIBDIR)"
	@echo "  vardir:      $(VARDIR)"
	@echo "  mandir:      $(MANDIR)"
	@echo "  privatedir:  $(PRIVATE_DIR)"
	@echo "  configdir:   $(CONFIGDIR)"
	@echo "  lockdir:     $(LOCKDIR)"
	@echo "  piddir:      $(PIDDIR)"
	@echo "  swatdir:     $(SWATDIR)"
