#
# $Id: Makefile,v 1.1 2007/06/01 20:59:02 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	
	

LDFLAGS+= -lassistcgi

CGIS =	uploadFirmware.cgi updateFirmware.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

uploadFirmware.cgi:firmwareUpload.c 
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   

updateFirmware.cgi:firmwareUpdate.c 
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   
