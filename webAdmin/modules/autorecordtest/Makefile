#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

C_FLAGS += 

LDFLAGS+= -lcgi -lShared -lpthread

# name of all files to be compiled (don't include header files)
SRC = \
	recordtest.c
	
CGIS = autorecordtest

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

autorecordtest: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/autorecordtest   
