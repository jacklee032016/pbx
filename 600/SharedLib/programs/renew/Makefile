# $Id: Makefile,v 1.1.1.1 2006/11/29 08:35:30 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

SRC= \
	renew.c 

CODE_SUBDIRS = .

LOCAL_ALL := renew 

include $(ROOT_DIR)/Makefile.post

LDFLAGS+=  -L$(BIN_DIR)/lib -laslib -lpthread -lm 

renew:$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
