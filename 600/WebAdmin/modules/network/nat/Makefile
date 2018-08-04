#
# $Id: Makefile,v 1.3 2007/08/31 20:30:50 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = \
	nat \
	track \
	lookup \
	middle \
	expert \
	timer \
	qos

#	vpn \

# name of all files to be compiled (don't include header files)
#	vpn/fwVpn.c \
#	vpn/vpnClient.c \

SRC = \
	qos/netQos.c \
	timer/fwTimerButtons.c \
	timer/fwTimer.c \
	expert/fwExpertButtons.c \
	expert/fwExpert.c \
	middle/fwMiddleCfg.c \
	middle/fwMiddleButtons.c \
	middle/fwMiddle.c \
	lookup/fwParseCurrent.c \
	lookup/fwLookup.c \
	track/fwConnTrack.c \
	nat/fwNatCfg.c \
	nat/fwNatButtons.c \
	nat/fwNat.c \
	fwButtons.c \
	fwOps.c \
	fwUtils.c \
	fwMain.c


LDFLAGS+= -lassistcgi

CGIS = firewall.cgi

LOCAL_ALL:=$(CGIS) 

include $(ROOT_DIR)/Makefile.post

firewall.cgi: $(OBJS)
	$(CC) -o $@ $(C_FLAGS) $^ $(LDFLAGS) 
	$(STRIP) $@
	$(MOVE) $@ $(CGI_BIN_DIR)/network   
