# Makefile for the Assist Telephone Echo Canceler driver
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
#


EXTRA_CFLAGS +=  -I. -I../ -I../include -Idsp/user -I../../include  

ifeq ($(CONFIG_ASSIST_PHONE_SLIC_DEBUG),m)

	EXTRA_CFLAGS += -DAS_SLIC_DEBUG=1    
endif


ifeq ($(CONFIG_ASSIST_PHONE_HW_DSP),m)

	EXTRA_CFLAGS += -DAS_PROSLIC_DSP=1 
endif

				
ifeq ($(CONFIG_ASSIST_PHONE_SLIC_TIGER_DEBUG),m)

	EXTRA_CFLAGS += -DAS_DEBUG_TIGER=1    
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


EXTRA_CFLAGS +=  -D__ARM__     


# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.
export-objs	:=  as_tel_init.o 
				
# Multipart objects.


list-multi	:= assist_slic_span.o
assist-objs	:= 	as_tel_init.o \
			file/as_dev_file_rw.o \
			file/as_dev_file_open.o \
			file/as_dev_file_poll.o \
			io/as_dev_io_receive.o \
			io/as_dev_io_transmit.o \
			ioctl/as_dev_file_ctl.o \
			ioctl/as_dev_ctl_ch_and_pseudo.o \
			ioctl/as_span_ctl.o \
			law/as_tel_law.o \
			utils/as_dev_utils.o \
			utils/as_dev_chan_utils.o \
			utils/as_dev_event_queue.o \
			dsp/as_dsp_zone.o \
			dsp/as_dsp_dsp.o  \
			misc/as_delay_switch.o
				
ifeq ($(CONFIG_ASSIST_PHONE_SLIC_SOFT_ECHO),m)

	assist-objs	+= echo/as_echo_cancel.o 
	EXTRA_CFLAGS 	+= -DAS_SOFT_ECHO=1    
endif




obj-$(CONFIG_ASSIST_PHONE_SLIC)		+= $(list-multi)

# The global Rules.make.
include $(TOPDIR)/Rules.make


# Link rules for multi-part drivers.

#assist_hfcm.o: $(assist-objs)
$(list-multi): $(assist-objs)
	$(LD) -r -o $@ $(assist-objs)
	mv $(list-multi) ../..
