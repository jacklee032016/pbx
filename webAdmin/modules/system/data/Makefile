#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiDataUtils.c \
	cgiDataRestore.c \
	cgiDataBackup.c \
	cgiDataMain.c 

LDFLAGS+= -lcgi

CGIS = \
	sysData.cgi 

#webadmin_main
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post


sysData.cgi:$(OBJS)  
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system   

