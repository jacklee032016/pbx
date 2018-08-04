#
# $Id: Makefile,v 1.1 2007/03/26 09:08:04 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	alert_main.c 
	

LDFLAGS+= -lassistcgi

CGIS =	alert 
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

alert: $(OBJS) 
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
