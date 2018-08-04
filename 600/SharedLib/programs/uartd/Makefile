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

LDFLAGS += -lpthread 

SRC:= \
		as_uartd_threads.c \
		as_uartd_main.c 

CODE_SUBDIRS = .

LOCAL_ALL := uartd 

include $(ROOT_DIR)/Makefile.post

LDFLAGS+=  -L$(BIN_DIR)/lib -laslib -lpthread -lm 

uartd:$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin  
