# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:58 lizhijie Exp $
# Makefile for the Assist ISDN Core drivers

ifdef CONFIG_ASSIST_MISDN_MEMDEBUG
	EXTRA_CFLAGS += -DISDN_MEMDEBUG
endif

EXTRA_CFLAGS +=  -I. -I../ -I../include 

# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= core.o helper.o debug.o fsm.o dchannel.o bchannel.o \
		l3helper.o memdbg.o as_raw_device.o as_file_ops.o 
				
# Multipart objects.

list-multi	:= assist_isdn_core.o
mcore-objs	:= core.o \
				stack.o \
				udevice.o \
				helper.o \
				fsm.o \
				dchannel.o \
				bchannel.o \
				as_file_ops.o \
				as_raw_device.o \
				as_frame_ops.o \
				as_frame_if.o \
				as_frame_timer.o \
				l3helper.o 
				
#				as_raw_ops.o \
				
# remove this file from objects. lizhijie, 2005.12.15 				
#				debug.o \

ifdef CONFIG_ASSIST_MISDN_MEMDEBUG
mcore-objs += memdbg.o
endif

# Ordering constraints: kernelcapi.o first

# Each configuration option enables a list of files.

obj-$(CONFIG_ASSIST_MISDN_CORE)+= $(list-multi)

# The global Rules.make.

include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.

$(list-multi): $(mcore-objs)
	$(LD) -r -o $@ $(mcore-objs)
	mv $(list-multi)  ../
