# $Id: Makefile,v 1.1 2006/12/19 14:23:21 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

SRC= \
	setup_flash.c 

CODE_SUBDIRS = .

LOCAL_ALL := setup  

include $(ROOT_DIR)/Makefile.post

LDFLAGS+=  -L$(BIN_DIR)/lib -laslib -lpthread -lm 

setup:$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/test 
