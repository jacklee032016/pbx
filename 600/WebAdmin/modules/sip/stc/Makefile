#
# $Id: Makefile,v 1.1 2007/04/24 19:31:02 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	stc_isp.c 
	

LDFLAGS+= -lassistcgi

CGIS =	isp 
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

isp: $(OBJS) 
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
