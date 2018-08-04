#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/29 08:35:30  lizhijie
# AS600 Shared Library
#
# Revision 1.1.1.1  2006/07/12 09:12:53  lizhijie
# rebuild as600 shared library
#
#


include $(ROOT_DIR)/Makefile.pre

SRC= 	as_test_main.c \
		as_test_result.c \
		as_test_drivers.c \
		as_test_pcm_channels.c \
		as_test_net_link.c \
		as_test_dev_cf_card.c \
		as_test_serial_no.c

CODE_SUBDIRS = .

LOCAL_ALL := selftest 

include $(ROOT_DIR)/Makefile.post

LDFLAGS+=  -L$(BIN_DIR)/lib -laslib -lpthread -lm 

selftest:$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin

