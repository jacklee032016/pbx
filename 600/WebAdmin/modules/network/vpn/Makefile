#
# $Id: Makefile,v 1.1 2007/08/31 20:31:59 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC = \
	cgiVpnStatus.c \
	cgiVpnAccount.c \
	cgiVpnAccountAs600.c \
	cgiVpnServer.c \
	cgiVpnClient.c \
	cgiVpnMain.c

LDFLAGS+= -lassistcgi

CGIS = vpn.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

vpn.cgi: $(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
