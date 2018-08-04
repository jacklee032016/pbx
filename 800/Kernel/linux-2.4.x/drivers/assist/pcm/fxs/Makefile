# Makefile for the Assist Telephone Echo Canceler driver
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
#


EXTRA_CFLAGS +=  -I. -I../ -I../include  
EXTRA_CFLAGS +=  -D__ARM__     

# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= assist_fxs.o
				
# Multipart objects.


list-multi	:= assist_pcm_fxs.o
assist-objs	:= 	as_fxs_init.o \
			as_fxs_device_init.o \
			as_fxs_isr.o   

ifneq ($(CONFIG_ASSIST_PHONE_PCM_TIGER_DEBUG),n)
	EXTRA_CFLAGS += -DAS_DEBUG_TIGER=1    
	assist-objs	+= as_fxs_tiger.o
endif

obj-$(CONFIG_ASSIST_PHONE_PCM)		+= $(list-multi)

# The global Rules.make.
include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.

#assist_hfcm.o: $(assist-objs)
$(list-multi): $(assist-objs)
	$(LD) -r -o $@ $(assist-objs)
	mv $(list-multi) ../..
