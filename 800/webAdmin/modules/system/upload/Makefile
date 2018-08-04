#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	admin_upload.c

LDFLAGS+= -lcgi

CGIS = \
	uploadAdmin.cgi

		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

uploadAdmin.cgi:$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system   
