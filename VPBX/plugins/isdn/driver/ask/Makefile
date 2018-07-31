#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
#

TOP_DIR:=$(shell pwd)/..


CFLAGS	+= -I$(TOP_DIR)/include 

include $(TOP_DIR)/Makefile.pre

CODE_SUBDIRS = 

SRC= 	ask_device.c \
	ask_layer.c \
	ask_stack.c \
	ask_status.c
	
LOCAL_CLEAN:=clear

LOCAL_ALL:


$(LOCAL_CLEAN):
	
TARGET_LIB:=libaskisdn
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

include $(TOP_DIR)/Makefile.post
