#
# $Id: Makefile,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += 

LDFLAGS+= -lassistcgi

CGIS =	www.cgi uploadWww.cgi 
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

www.cgi:www_cgi.c  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/servers   

uploadWww.cgi:www_upload.c 
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/servers   

