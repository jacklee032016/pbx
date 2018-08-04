#
# $Id: Makefile,v 1.2 2007/06/22 20:31:00 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	hwCheckRinger.c \
	hwCheckPcm.c \
	hwCheckNvRam.c \
	hwCheckNvDtmf.c \
	hwCheckUtils.c \
	hwCheckMain.c
	

LDFLAGS+= -lassistcgi

CGIS =	pbxCheck.cgi 
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

pbxCheck.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   
