#
# $Id: Makefile,v 1.1 2007/05/30 19:20:19 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = . 

# name of all files to be compiled (don't include header files)
SRC += \
	../md5.c \
	../ddnsBase64.c \
	../ddnsBitStream.c \
	../ddnsCodec.c \
	../ddnsBlowFish.c \
	ddnsTestBlow.c 

#	../blowFish.c \

C_FLAGS += -I../
LDFLAGS+= -laslib -lpthread -lm

CGIS = \
	blowtest

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

blowtest:$(OBJS)  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/sbin 
