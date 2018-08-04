#
# $Id: Makefile,v 1.1 2007/05/31 20:39:35 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiPeanutConfig.c \
	cgiPeanutTest.c \
	cgiPeanutMain.c 

	

LDFLAGS+= -lassistcgi

CGIS =	ddns.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

ddns.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/services    

