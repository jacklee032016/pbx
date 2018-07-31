#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = . 

# name of all files to be compiled (don't include header files)
SRC += \
	syscfgMain.c \
	syscfg_hostname.c \
	syscfg_eth.c \
	syscfg_reboot.c \
	syscfg_pppoe.c \
	syscfg_user.c \
	syscfg_descrip.c
	

C_FLAGS +=   
LDFLAGS+= -lcgi

CGIS = \
	sysconf.cgi 

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

sysconf.cgi:$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system  

