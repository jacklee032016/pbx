# $Id: Makefile,v 1.2 2007/03/25 19:56:17 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

LDFLAGS += -lpthread 

CODE_SUBDIRS = .

SRC:= 	\
		as_record_main.c 

#		as_prog_taper.c \

LOCAL_ALL := recorder 

include $(ROOT_DIR)/Makefile.post

LDFLAGS+=  -L$(BIN_DIR)/lib -laslib -lpthread -lm 


recorder:$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin 
