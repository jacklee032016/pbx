#
# $Id: Makefile,v 1.3 2007/03/11 20:19:10 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	pbx_cgi_command.c \
	pbx_send_cmds.c \
	pbx_setup_pbx.c \
	pbx_serial_thread.c \
	pbx_main.c 
	

LDFLAGS+= -lassistcgi

CGIS =	cgipbx 
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

cgipbx: $(OBJS) 
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
