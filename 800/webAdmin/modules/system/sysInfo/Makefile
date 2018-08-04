#
# $Id$
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

LDFLAGS+= -lcgi

CGIS = \
	sysInfo.cgi 

		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post


sysInfo.cgi:$(OBJS)  
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system   


