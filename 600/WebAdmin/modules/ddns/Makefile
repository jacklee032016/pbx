#
# $Id: Makefile,v 1.4 2007/06/01 18:43:07 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = . 

# name of all files to be compiled (don't include header files)
SRC += \
	md5.c \
	ddnsBase64.c \
	ddnsBlowFish.c \
	ddnsBitStream.c \
	ddnsCodec.c \
	ddnsHeartbeat.c \
	ddnsShakehand.c \
	ddnsMain.c 

# C_FLAGS += -I./upload  
LDFLAGS+= -laslib -lpthread -lm -lassistcgi

CGIS = \
	ddnsd

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

ddnsd:$(OBJS)  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
