# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:58 lizhijie Exp $
# Makefile for the kernel Assist Board subsystem and device drivers.

# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= 

# Multipart objects.

list-multi	:= 
isdn-objs	:= 

# Optional parts of multipart objects.

# Ordering constraints: isdn.o first, rest doesn't matter

# Each configuration option enables a list of files.

obj-$(CONFIG_ASSIST_BOARD)		+= assist.o
obj-$(CONFIG_ASSIST_ISDN)		+= assist_isdn.o

# Object files in subdirectories

mod-subdirs	:= devices/assist isdn \
		slic pcm zarlink \
		misc 
		  
subdir-$(CONFIG_ASSIST_ISDN)		+= isdn
subdir-$(CONFIG_ASSIST_PHONE_SLIC)	+= slic
subdir-$(CONFIG_ASSIST_PHONE_PCM)	+= pcm
subdir-$(CONFIG_ASSIST_ECHO_CANCEL)	+= zarlink 
subdir-$(CONFIG_ASSIST_MISC)		+= misc


obj-y += $(addsuffix /vmlinux-obj.o, $(subdir-y))

# The global Rules.make.

include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.
