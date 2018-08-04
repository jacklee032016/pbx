# Makefile for the Assist ISDN HFC-4S PCI card driver
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:58 lizhijie Exp $
#

ifdef CONFIG_ASSIST_MISDN_MEMDEBUG
	EXTRA_CFLAGS += -DISDN_MEMDEBUG
endif

ifdef CONFIG_ASSIST_HFC_PORT_4
	EXTRA_CFLAGS += -DAS_HFC_PORTS=4 
else 
	ifdef CONFIG_ASSIST_HFC_PORT_3
		EXTRA_CFLAGS += -DAS_HFC_PORTS=3 
	else 
		ifdef CONFIG_ASSIST_HFC_PORT_2
			EXTRA_CFLAGS += -DAS_HFC_PORTS=2 
		else 
			ifdef CONFIG_ASSIST_HFC_PORT_1 
				EXTRA_CFLAGS += -DAS_HFC_PORTS=1 
			else	
				EXTRA_CFLAGS += -DAS_HFC_PORTS=0  
			endif
		endif
	endif			
endif

ifdef CONFIG_ASSIST_HFC_PORT_4
	ifeq ($(CONFIG_ASSIST_HFC_PORT4_NT),y)
		EXTRA_CFLAGS += -DAS_HFC_PORT_4_NT=1 
	else
		EXTRA_CFLAGS += -DAS_HFC_PORT_4_NT=0 
	endif
endif

ifdef CONFIG_ASSIST_HFC_PORT_3
	ifeq ($(CONFIG_ASSIST_HFC_PORT3_NT),y)
		EXTRA_CFLAGS += -DAS_HFC_PORT_3_NT=1 
	else
		EXTRA_CFLAGS += -DAS_HFC_PORT_3_NT=0 
	endif
endif

ifdef CONFIG_ASSIST_HFC_PORT_2
	ifeq ($(CONFIG_ASSIST_HFC_PORT2_NT),y)
		EXTRA_CFLAGS += -DAS_HFC_PORT_2_NT=1 
	else
		EXTRA_CFLAGS += -DAS_HFC_PORT_2_NT=0 
	endif
endif

ifdef CONFIG_ASSIST_HFC_PORT_1
	ifeq ($(CONFIG_ASSIST_HFC_PORT1_NT),y)
		EXTRA_CFLAGS += -DAS_HFC_PORT_1_NT=1 
	else
		EXTRA_CFLAGS += -DAS_HFC_PORT_1_NT=0 
	endif
endif


EXTRA_CFLAGS +=  -I. -I../ -I../include -I../dsp/dsp 

EXTRA_CFLAGS += -DWITH_HW=1 -DCONFIG_HFCMULTI_PCIMEM=0    

# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

export-objs	:= 
				
# Multipart objects.


list-multi	:= assist_isdn_hfc.o
assist-objs	:= 	ashfc_bh.o \
				ashfc_isr.o \
				ashfc_hw_init.o \
				ashfc_hw_utils.o \
				ashfc_signal.o \
				ashfc.o \
				ashfc_init_utils.o \
				ashfc_init.o \
				ashfc_ioctl.o 

# Ordering constraints: 

# Each configuration option enables a list of files.

obj-$(CONFIG_ASSIST_MISDN_HFCMULTI)		+= $(list-multi)

# The global Rules.make.

include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.

#assist_hfcm.o: $(assist-objs)
$(list-multi): $(assist-objs)
	$(LD) -r -o $@ $(assist-objs)
	mv $(list-multi) ../
