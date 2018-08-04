#
# $Id: Makefile,v 1.2 2006/12/11 09:24:29 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	lcdd_usb.c \
	lcdd_utils.c \
	lcdd_main.c

LDFLAGS+= -lassistcgi

CGIS =	lcdd
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

lcdd:$(OBJS)  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin    

