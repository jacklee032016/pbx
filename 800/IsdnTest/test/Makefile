#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:21:59 lizhijie Exp $
#

TOP_DIR:=$(shell pwd)/..

include $(TOP_DIR)/Makefile.pre

CFLAGS += -I./include 

LDFLAGS	+= -L$(BIN_DIR) -lpthread 

CODE_SUBDIRS = lib ask 

LOCAL_CLEAN:=clear

LOCAL_ALL:

$(LOCAL_CLEAN):

SRC= 	\
	lib/devutils.c \
	lib/as_thread.c \
	lib/rawdev_thread.c \
	ask/ask_device.c \
	ask/ask_layer.c \
	ask/ask_stack.c \
	ask/ask_status.c

TARGET_LIB:=libisdn_test
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

LOCAL_ALL := ntcall tecall 

LOCAL_ALL:TRAGET_LIB

ntcall:nt_call.c 
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) -lisdn_test    
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)

tecall:te_call.c 
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) -lisdn_test    
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)
	
include $(TOP_DIR)/Makefile.post
