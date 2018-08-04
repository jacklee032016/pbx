#
# $Id: Makefile,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	main.c 

LDFLAGS+= -lassistcgi

CGIS =	reconfig
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

reconfig:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/sbin   

