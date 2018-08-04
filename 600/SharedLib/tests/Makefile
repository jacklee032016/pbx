# $Log: Makefile,v $
# Revision 1.2  2006/12/03 08:32:37  lizhijie
# d
#
# Revision 1.1.1.1  2006/11/29 08:35:30  lizhijie
# AS600 Shared Library
#
# Revision 1.1.1.1  2006/07/12 09:12:53  lizhijie
# rebuild as600 shared library
#
# $Id: Makefile,v 1.2 2006/12/03 08:32:37 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = . 


SRC = 	\
		test_utils.c  \
		test_beep.c \
		test_button.c \
		test_watchdog.c \
		test_serialno.c 
	

SPROGS=	\
		test_log \
		test_beep \
		test_button \
		test_watchdog \
		test_serialno 

# AS600…œ√ª”–EEPROM–æ∆¨
ifeq ($(WITH_93LC66),yes)
	SRC += \
		test/test_write_93lc66.c \
		test/test_read_93lc66.c
		
	SPROGS += \
		test_write_93lc66 \
		test_read_93lc66
endif

LOCAL_ALL=$(SPROGS)


LOCAL_CLEAN:=clear

$(LOCAL_CLEAN):
	
TARGET_LIB:=

LDFLAGS+=  -L$(BIN_DIR)/lib -laslib -lpthread -lm 

include $(ROOT_DIR)/Makefile.post

test_log: $(OBJ_DIR)/test_utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@ 
	$(MOVE) $@ $(BIN_DIR)/usr/test
	
test_beep:$(OBJ_DIR)/test_beep.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@ 
	$(MOVE) $@ $(BIN_DIR)/usr/test

test_button:$(OBJ_DIR)/test_button.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@ 
	$(MOVE) $@ $(BIN_DIR)/usr/test
	
test_watchdog:$(OBJ_DIR)/test_watchdog.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@ 
	$(MOVE) $@ $(BIN_DIR)/usr/test 
	
test_serialno:$(OBJ_DIR)/test_serialno.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@ 
	$(MOVE) $@ $(BIN_DIR)/usr/test
	 
ifeq ($(WITH_93LC66),yes)
test_write_93lc66:$(OBJ_DIR)/test_write_93lc66.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@ 
	$(MOVE) $@ $(BIN_DIR)/usr/test
	
test_read_93lc66:$(OBJ_DIR)/test_read_93lc66.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@ 
	$(MOVE) $@ $(BIN_DIR)/usr/test
endif
