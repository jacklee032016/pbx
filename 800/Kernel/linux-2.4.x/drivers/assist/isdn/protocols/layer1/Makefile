# Makefile for the Assist ISDN Layer-1 drivers
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:58 lizhijie Exp $
#

ifdef CONFIG_ASSIST_MISDN_MEMDEBUG
	EXTRA_CFLAGS += -DISDN_MEMDEBUG
endif

EXTRA_CFLAGS +=  -I. -I../.. -I../../include 

# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= 
				
# Multipart objects.

list-multi	:= assist_isdn_l1.o
mlayer1-objs	:= 	layer1.o

# Ordering constraints: 

# Each configuration option enables a list of files.

obj-$(CONFIG_ASSIST_MISDN_L1)		+= $(list-multi)

# The global Rules.make.

include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.

$(list-multi): $(mlayer1-objs)
	$(LD) -r -o $@ $(mlayer1-objs)
	mv $(list-multi) ../../
