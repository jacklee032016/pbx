#
# $Id: Makefile,v 1.1 2007/03/30 09:09:03 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

C_FLAGS +=  

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiCdrMain.c 
	
LDFLAGS+= -lassistcgi

LDFLAGS+= 

CGIS =	cdr.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

cdr.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   

