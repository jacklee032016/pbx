#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	wizardUtils.c \
	wizardMain.c 

#	wizardWan.c \
#	wizardHardware.c \
#	wizardFdisk.c \
#	wizardFormat.c \
#	wizardUpload.c \


LDFLAGS+= -lcgi

CGIS =	wizard.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

wizard.cgi:$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)   

