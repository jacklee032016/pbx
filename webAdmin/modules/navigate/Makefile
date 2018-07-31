#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += cgi_navigate.c 

CGIS =	navigate.cgi content.cgi 
# index.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

C_FLAGS +=
# -fPIC 
LDFLAGS += -lcgi

navigate.cgi:$(OBJS)  
	$(CC) -o $@ $^ $(LDFLAGS)
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
