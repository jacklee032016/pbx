#
# $Id: Makefile,v 1.2 2006/12/19 17:32:00 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = help 

# name of all files to be compiled (don't include header files)
SRC += 

C_FLAGS +=  
LDFLAGS+= -lassistcgi

CGIS = \
	wtd \
	help.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

wtd:wtd.c
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin   

help.cgi:help/help_cgi.c help/help_msg.c  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)  
