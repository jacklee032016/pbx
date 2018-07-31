#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	servicesMiscConfig.c \
	servicesMiscUpnp.c \
	servicesMiscIpset.c \
	servicesMiscSysLog.c \
	servicesMiscMain.c 

#	servicesMiscFtp.c \
	

LDFLAGS+= -lcgi

CGIS =	servicesmisc.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

servicesmisc.cgi:$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/services    

