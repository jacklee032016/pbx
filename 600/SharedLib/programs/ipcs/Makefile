# $Id: Makefile,v 1.1 2007/03/24 18:35:39 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

SRC= \
	ipcs.c 

CODE_SUBDIRS = .

LOCAL_ALL := ipcs 

include $(ROOT_DIR)/Makefile.post

LDFLAGS+=  -L$(BIN_DIR)/lib -laslib -lpthread -lm 

ipcs:$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
