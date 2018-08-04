#
# $Id: Makefile,v 1.6 2007/09/03 21:35:49 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	sipUa_disturb_list.c \
	sipUa_dialrule_utils.c \
	sipUa_dial_rules.c \
	sipUa_dial_rules.c \
	sipUa_dial_parttern.c \
	sipUa_blacklist_mgmt.c \
	sipUa_protocol.c \
	sipUa_server.c \
	sipUa_GlobalPhone.c \
	sipUa_PstnStatus.c \
	sipUa_GainControl.c \
	sipUa_main.c 
	

LDFLAGS+= -lassistcgi

CGIS =	sipUa.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

sipUa.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   

