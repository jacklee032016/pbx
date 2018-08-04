#
# $Id: Makefile,v 1.3 2007/01/24 16:38:28 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

C_FLAGS += 

LDFLAGS+= -lassistcgi 

# name of all files to be compiled (don't include header files)
SRC = \
	videoView.c \
	videoRecord.c \
	videoMain.c

CGIS = video.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

video.cgi: $(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
