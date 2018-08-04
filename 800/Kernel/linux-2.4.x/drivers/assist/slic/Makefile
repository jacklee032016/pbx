# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
# Makefile for the kernel Assist ISDN subsystem and device drivers.

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
