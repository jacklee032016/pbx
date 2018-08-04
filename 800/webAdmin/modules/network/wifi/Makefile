#
# $Id$
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

C_FLAGS += -I$(ROOT_DIR)/common_Wifi/tools/lib

# name of all files to be compiled (don't include header files)
SRC = \
	wifiMain.c

#	wifiInfo.c \
#	wifiAodvInfo.c \
#	wifiAodvCtrl.c \
#	wifiAcl.c \
#	wifiAuthen.c \
#	wifiStations.c \

LDFLAGS+= -L$(ROOT_DIR)/common_Wifi/Binary/lib -lcgi 
# -lwifi 

CGIS = wifi.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

wifi.cgi: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
