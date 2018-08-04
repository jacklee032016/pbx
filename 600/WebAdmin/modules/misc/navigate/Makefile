#
# $Id: Makefile,v 1.2 2006/12/19 14:33:35 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += cgi_navigate.c 

LDFLAGS+= -lassistcgi
C_FLAGS += -I$(ROOT_DIR)/common_Wifi/tools/lib

CGIS =	navigate.cgi content.cgi 
# index.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

navigate.cgi:$(OBJS)  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)   

content.cgi:content_cgi.c
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)  

# index.cgi:index_cgi.c
#	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
#	$(STRIP) $@
#	$(MOVE) $@ $(CGI_BIN_DIR)  
