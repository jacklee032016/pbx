#
# $Id: Makefile,v 1.5 2007/06/14 08:15:14 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

C_FLAGS += -I$(ROOT_DIR)/common_SipServer/partysip/include 
C_FLAGS += -I$(ROOT_DIR)/common_SipServer/partysip/include/ppl 

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	partyFilters.c \
	partyUsers.c \
	partyGlobals.c \
	partyMain.c \
	partyLookup.c \
	partySave.c 
	
LDFLAGS+= -lassistcgi

LDFLAGS+= -L$(ROOT_DIR)/common_SipServer/binary  -losipparser2 -lppl -losip2 -lgdbm -lresolv -lnsl -ldl -pthread

CGIS =	sipServer.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

sipServer.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   

