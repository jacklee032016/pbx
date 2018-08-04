#
# $Id: Makefile,v 1.2 2006/12/19 14:33:35 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
		disk_format.c \
		disk_fdisk.c \
		disk_install.c \
		disk_management.c \
		cgi_disk.c \
		disk_cgi.c 

LDFLAGS+= -lassistcgi

CGIS =	disk.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

disk.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system   

