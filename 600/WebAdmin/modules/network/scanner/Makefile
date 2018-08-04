#
# $Id: Makefile,v 1.3 2007/02/07 10:06:31 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

C_FLAGS += -I$(ROOT_DIR)/common_Epson/lib -I$(ROOT_DIR)/comLib_JPeg/include

LDFLAGS+= -lassistcgi -L$(ROOT_DIR)/common_Epson/Binary/lib 
LDFLAGS+= -L$(ROOT_DIR)/comLib_Usb/Binary/lib -lasscan -lsane-epson -lsanei -lusb
LDFLAGS+= -L$(ROOT_DIR)/comLib_JPeg/bin -ljpeg -lcjpeg 

# name of all files to be compiled (don't include header files)
SRC = \
	scanSetup.c \
	scanScan.c \
	scanLoad.c \
	scanMain.c

CGIS = scanner.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

scanner.cgi: $(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
