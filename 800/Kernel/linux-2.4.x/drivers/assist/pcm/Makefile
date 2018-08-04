# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
# Makefile for the kernel Assist ISDN subsystem and device drivers.

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

obj-$(CONFIG_ASSIST_PHONE_SLIC)		+= assist_span.o

# Object files in subdirectories

mod-subdirs	:= 	\
		span \
		fxs 
		  
subdir-$(CONFIG_ASSIST_PHONE_SLIC)		+= span 
subdir-$(CONFIG_ASSIST_PHONE_SLIC)		+= fxs 

obj-y += $(addsuffix /vmlinux-obj.o, $(subdir-y))

# The global Rules.make.

include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.
