# Makefile for the Assist Telephone Echo Canceler driver
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:28:00 lizhijie Exp $
#

EXTRA_CFLAGS +=  -I. -I../ -I../include  

ifeq ($(CONFIG_ASSIST_ECHO_16CH),m)
	EXTRA_CFLAGS += -DAS_HW_EC_50235=1 
endif

# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= as_zl_if.o 
				
# Multipart objects.


list-multi	:= assist_echo.o
assist-objs	:= 	as_zl_init.o \
			as_zl_zarlink.o \
			as_zl_if.o 
				

# Ordering constraints: 

# Each configuration option enables a list of files.

obj-$(CONFIG_ASSIST_ECHO_CANCEL)		+= $(list-multi)

# The global Rules.make.

include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.

#assist_hfcm.o: $(assist-objs)
$(list-multi): $(assist-objs)
	$(LD) -r -o $@ $(assist-objs)
	mv $(list-multi) ../
