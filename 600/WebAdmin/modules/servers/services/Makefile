#
# $Id: Makefile,v 1.3 2007/08/31 20:30:50 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	servicesMiscConfig.c \
	servicesMiscFtp.c \
	servicesMiscRip.c \
	servicesMiscUpnp.c \
	servicesMiscSysLog.c \
	servicesMiscMain.c 

#	servicesMiscVPNServer.c \
#	servicesMiscDhcp.c \
	

LDFLAGS+= -lassistcgi

CGIS =	servicesmisc.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

servicesmisc.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/services    

