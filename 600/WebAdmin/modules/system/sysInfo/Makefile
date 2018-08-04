#
# $Id: Makefile,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	sysInfoTime.c \
	sysInfoLocale.c \
	sysInfoVersions.c \
	sysInfoProcs.c \
	sysInfoMain.c 

LDFLAGS+= -lassistcgi

CGIS = \
	sysInfo.cgi 

		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post


sysInfo.cgi:$(OBJS)  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system   


