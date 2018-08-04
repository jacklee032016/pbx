#
# $Id: Makefile,v 1.2 2006/12/04 08:56:17 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = . 

# name of all files to be compiled (don't include header files)
SRC += \
	peanut_main.c \
	peanut_md5.c 

# C_FLAGS += -I./upload  
LDFLAGS+= -lassistcgi -laslib

CGIS = \
	peanutd

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

peanutd:$(OBJS)  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/sbin 
