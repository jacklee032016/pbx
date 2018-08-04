#
# $Id: Makefile,v 1.4 2007/01/15 19:58:47 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	wizardUtils.c \
	wizardWan.c \
	wizardHardware.c \
	wizardFdisk.c \
	wizardFormat.c \
	wizardUpload.c \
	wizardMain.c 

LDFLAGS+= -lassistcgi

CGIS =	wizard.cgi setdeft
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

wizard.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)   

setdeft:set_default.c
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin   
