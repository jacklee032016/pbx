#
# $Id: Makefile,v 1.2 2006/12/19 17:32:00 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

# name of all files to be compiled (don't include header files)
SRC += \
	cgiDataUtils.c \
	cgiDataRestore.c \
	cgiDataBackup.c \
	cgiDataMain.c 

LDFLAGS+= -lassistcgi

CGIS = \
	sysData.cgi \
	restore_deft \
	restore_tftp \
	uploadAdmin.cgi

#webadmin_main
		 
LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post


sysData.cgi:$(OBJS)  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system   

uploadAdmin.cgi:admin_upload.c  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/system   

restore_tftp:restore_tftp.c  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

restore_deft:restore_deft.c  
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

