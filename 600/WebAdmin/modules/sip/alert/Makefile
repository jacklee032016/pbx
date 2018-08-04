#
# $Id: Makefile,v 1.2 2007/03/26 21:18:32 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

C_FLAGS +=  

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiAlertSetup.c \
	cgiAlertRecord.c \
	cgiAlertUtils.c \
	cgiAlertMain.c 
	
LDFLAGS+= -lassistcgi

LDFLAGS+= 

CGIS =	alert.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

alert.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   

