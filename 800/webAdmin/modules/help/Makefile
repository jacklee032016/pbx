#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	help_cgi.c \
	help_msg.c \
	

CGIS =	help.cgi 
# index.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

C_FLAGS +=
# -fPIC 
LDFLAGS += -lcgi

help.cgi:$(OBJS)  
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)   

