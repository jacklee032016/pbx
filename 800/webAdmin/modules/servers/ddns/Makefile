#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiDdnsConfig.c \
	cgiDdnsTest.c \
	cgiDdnsMain.c \
	ddnsUtils.c 

	

LDFLAGS+= -lcgi

CGIS =	ddns.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

ddns.cgi:$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/services    

