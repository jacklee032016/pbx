# Makefile for the Assist Telephone Echo Canceler driver
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
#


EXTRA_CFLAGS +=  -I. -I../ -I../include  -I../../include  
EXTRA_CFLAGS +=  -D__ARM__     

ifeq ($(CONFIG_ASSIST_PHONE_SLIC_DEBUG),m)

	EXTRA_CFLAGS += -DAS_SLIC_DEBUG=1    
endif


ifeq ($(CONFIG_ASSIST_PHONE_JATE),m)

	EXTRA_CFLAGS += -DAS_WITH_JATE=1    
endif


ifeq ($(CONFIG_ASSIST_PHONE_SLIC_SOFT_ECHO),m)

	EXTRA_CFLAGS += -DAS_SOFT_ECHO=1    
endif



ifeq ($(CONFIG_ASSIST_PHONE_SLIC_SOFT_GAIN),m)

	EXTRA_CFLAGS += -DAS_SOFT_GAIN=1    
endif


ifeq ($(CONFIG_ASSIST_PHONE_SLIC_PULSE_DIALING),m)

	EXTRA_CFLAGS += -DAS_PULSE_DIALING=1    
endif


# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= as_fxs_init.o
				
# Multipart objects.


list-multi	:= assist_slic_fxs.o
assist-objs	:= 	as_fxs.o \
			as_fxs_init.o \
			as_fxs_device_init.o \
			as_fxs_isr.o \
			as_fxs_proslic.o \
			as_fxs_voicedaa.o \
			as_fxs_handler.o \
			as_fxs_dtmf.o  \
			$(shell pwd)/../span/misc/as_delay_switch.o
		
ifeq ($(CONFIG_ASSIST_PHONE_SLIC_TIGER_DEBUG),m)

	assist-objs	+= as_fxs_tiger.o
	EXTRA_CFLAGS += -DAS_DEBUG_TIGER=1    
endif


ifeq ($(CONFIG_ASSIST_PHONE_HW_DSP),m)

	assist-objs	+= as_fxs_tone.o
	EXTRA_CFLAGS += -DAS_PROSLIC_DSP=1 
endif

				

obj-$(CONFIG_ASSIST_PHONE_SLIC)		+= $(list-multi)

# The global Rules.make.
include $(TOPDIR)/Rules.make


# Link rules for multi-part drivers.

#assist_hfcm.o: $(assist-objs)
$(list-multi): $(assist-objs)
	$(LD) -r -o $@ $(assist-objs)
	mv $(list-multi) ../..
