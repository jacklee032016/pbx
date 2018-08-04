#
# $Id: Makefile,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

C_FLAGS += -I$(ROOT_DIR)/common_Wifi/tools/lib

# name of all files to be compiled (don't include header files)
SRC = \
	meshPortalCtrl.c \
	meshFwdCtrl.c \
	meshStatusInfo.c \
	meshMain.c


LDFLAGS+= -L$(ROOT_DIR)/common_Wifi/Binary/lib -lassistcgi -lwifi 

CGIS = mesh.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

mesh.cgi: $(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
