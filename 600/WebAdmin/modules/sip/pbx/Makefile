#
# $Id: Makefile,v 1.3 2007/05/16 18:35:40 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiPbxSpecial.c \
	cgiPbxAccounts.c \
	cgiPbxTrunck.c \
	cgiPbxLines.c \
	cgiPbxBranchPhone.c \
	cgiPbxBranchSpec.c \
	cgiPbxBranchBasic.c \
	cgiPbxBasicConfig.c \
	cgiPbxGlobalDialPlan.c \
	cgiPbxBranchCallTransfer.c \
	cgiPbxBranchExtTransfer.c \
	cgiPbxBranchNightService.c \
	cgiPbxBlackList.c \
	cgiPbxMain.c 
	

LDFLAGS+= -lassistcgi

CGIS =	pbxClient.cgi
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

pbxClient.cgi:$(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/sip   
