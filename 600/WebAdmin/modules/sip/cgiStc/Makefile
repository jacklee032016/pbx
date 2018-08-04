#
# $Id: Makefile,v 1.1 2007/05/02 21:09:12 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiStcCfgData.c \
	cgiStcCommand.c \
	cgiStcSetup.c \
	cgiStcTxCmds.c \
	cgiStcUartThread.c \
	cgiStcMain.c 
	

LDFLAGS+= -lassistcgi

CGIS =	cgipbx 
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

cgipbx: $(OBJS) 
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
