#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
# AS800 VPBX
#
# Revision 1.1.1.1  2006/01/10 08:53:12  lizhijie
# VPBX for DUX project
#
# Revision 1.1  2005/12/25 10:47:30  lizhijie
# no message
#
# Revision 1.1.1.1  2005/12/17 10:42:37  lizhijie
# VPBX with plugin
#
# Revision 1.1.1.1  2005/11/21 07:38:19  lizhijie
# re-import vpbx module
#
# Revision 1.1.1.1  2005/11/14 03:18:15  lizhijie
# VPBX for phone
#
# Revision 1.1.1.1  2005/09/21 10:38:09  lizhijie
# ISDN PBX user program
#
# Revision 1.1.1.1  2005/09/05 12:54:34  lizhijie
# ISDN library for Assist
#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
#

include $(TOP_DIR)/Makefile.pre

LDFLAGS	+= -laskisdn 

CODE_SUBDIRS = 

LOCAL_CLEAN:=clear

LOCAL_ALL:

$(LOCAL_CLEAN):

SRC= ask_lib.c

TARGET_LIB:=libaskisdn_user
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
	
LOCAL_LAST := testcon testcon_l2 logger testnet #loadfirm 


include $(TOP_DIR)/Makefile.post

testcon:ask_test_con.c 
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) -laskisdn_user
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/bin 

testnet:ask_test_net.c
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) -laskisdn_user
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/bin 

testcon_l2:ask_test_l2.c ask_lib.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) -laskisdn_user
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/bin 

logger:ask_test_logger.c ask_lib.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) -laskisdn_user
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/bin 

# loadfirm:loadfirm.c 
#	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS) 
#	$(STRIP) $@
#	$(MOVE) $@ $(BIN_DIR)/bin 

