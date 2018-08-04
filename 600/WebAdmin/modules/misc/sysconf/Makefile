#
# $Id: Makefile,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = . 

# name of all files to be compiled (don't include header files)
SRC += \
	syscfgMain.c \
	syscfg_hostname.c \
	syscfg_wan.c \
	syscfg_misc.c \
	syscfg_pppoe.c \
	syscfg_user.c 
	
#	syscfg_lan.c \

C_FLAGS +=   
LDFLAGS+= -lassistcgi

CGIS = \
	sysconf.cgi 

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

sysconf.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)  

