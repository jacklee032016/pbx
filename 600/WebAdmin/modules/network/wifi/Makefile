#
# $Id: Makefile,v 1.4 2007/01/22 21:00:55 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = .

C_FLAGS += -I$(ROOT_DIR)/common_Wifi/tools/lib

# name of all files to be compiled (don't include header files)
SRC = \
	wifiInfo.c \
	wifiAodvInfo.c \
	wifiAodvCtrl.c \
	wifiAcl.c \
	wifiAuthen.c \
	wifiStations.c \
	wifiMain.c

#	wifiMode.c \
#	wifiNetwork.c \

LDFLAGS+= -L$(ROOT_DIR)/common_Wifi/Binary/lib -lassistcgi -lwifi 

CGIS = wifi.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

wifi.cgi: $(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
