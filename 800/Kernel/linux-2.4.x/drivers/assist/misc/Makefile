# Makefile for the Assist Telephone Echo Canceler driver
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
#

EXTRA_CFLAGS +=  -I. -I../ -I../include  

ifeq ($(CONFIG_ASSIST_MISC_BEEPER),m)
	assist-objs += as_misc_beep.o
	EXTRA_CFLAGS += -DAS_MISC_BEEP=1 
endif

ifeq ($(CONFIG_ASSIST_MISC_BUTTON),m)
	assist-objs += as_misc_button.o 
	EXTRA_CFLAGS += -DAS_MISC_BUTTON=1 
endif

ifeq ($(CONFIG_ASSIST_MISC_WATCHDOG),m)
	assist-objs += as_misc_watchdog.o
	EXTRA_CFLAGS += -DAS_MISC_WATCHDOG=1 
endif

ifeq ($(CONFIG_ASSIST_MISC_EEPROM),m)
	assist-objs += as_misc_93lc66.o  
	EXTRA_CFLAGS += -DAS_MISC_EEPROM=1 
endif

ifeq ($(CONFIG_ASSIST_MISC_LED),m)
	assist-objs += as_misc_138.o
	EXTRA_CFLAGS += -DAS_MISC_LED=1 
endif

ifeq ($(CONFIG_ASSIST_MISC_ONOFFHOOK),m)
	assist-objs += as_misc_onoffhook.o
	EXTRA_CFLAGS += -DAS_MISC_ONOFFHOOK=1 
endif

ifeq ($(CONFIG_ASSIST_MISC_SWITCH),m)
	assist-objs += as_misc_switch.o
	EXTRA_CFLAGS += -DAS_MISC_SWITCH=1 
endif


# The target object and module list name.

O_TARGET	:= vmlinux-obj.o

# Objects that export symbols.

# export-objs	:= assist_misc.o
				
# Multipart objects.


list-multi := assist_misc.o
assist-objs += as_misc_init.o 
assist-objs += as_misc_gpio_ctl.o 

obj-$(CONFIG_ASSIST_MISC) += $(list-multi)




# The global Rules.make.
include $(TOPDIR)/Rules.make

# Link rules for multi-part drivers.

$(list-multi): $(assist-objs)
	$(LD) -r -o $@ $(assist-objs)
	mv $(list-multi) ../
