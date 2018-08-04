# Makefile for the Assist Telephone Echo Canceler driver
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
#


EXTRA_CFLAGS +=  -I. -I../ -I../include   
EXTRA_CFLAGS +=  -D__ARM__     

#EXTRA_CFLAGS += -DWITH_HW=1 -DCONFIG_HFCMULTI_PCIMEM=0    

# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= as_tel_init.o 
				
# Multipart objects.


list-multi	:= assist_pcm_span.o
assist-objs	:= 	as_tel_init.o \
			file/as_dev_file_rw.o \
			file/as_dev_file_open.o \
			io/as_dev_io_receive.o \
			io/as_dev_io_transmit.o \
			ioctl/as_dev_file_ctl.o \
			utils/as_dev_utils.o \
			utils/as_dev_chan_utils.o \
			law/as_tel_gain.o 
				


obj-$(CONFIG_ASSIST_PHONE_PCM)		+= $(list-multi)

# The global Rules.make.
include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.

#assist_hfcm.o: $(assist-objs)
$(list-multi): $(assist-objs)
	$(LD) -r -o $@ $(assist-objs)
	mv $(list-multi) ../..
