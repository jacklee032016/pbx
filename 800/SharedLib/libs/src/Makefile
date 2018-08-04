# Makefile 
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $

include $(TOP_DIR)/Makefile.pre

ifeq ($(WITH_93LC66),no)
else
CODE_SUBDIRS += m93lc66
SRC += m93lc66/as_93lc66.c
TEST_SRC += test/test_write_93lc66.c 
TEST_SRC += test/test_read_93lc66.c
LOCAL_test += test_write_93lc66 
LOCAL_test += test_read_93lc66
TEST_WRITE_93LC66_OBJS = $(TEST_OBJ_DIR)/test_write_93lc66.o
TEST_READ_93LC66_OBJS = $(TEST_OBJ_DIR)/test_read_93lc66.o
endif


CODE_SUBDIRS += pbx 
CODE_SUBDIRS += uart 
CODE_SUBDIRS += utils 
CODE_SUBDIRS += version 
CODE_SUBDIRS += voice 
CODE_SUBDIRS += recorder
CODE_SUBDIRS += lcd 
CODE_SUBDIRS += serialno 
CODE_SUBDIRS += flash 

SRC += pbx/as_pbx_cmd.c
SRC += pbx/as_watchdog.c 
SRC += pbx/as_msg_queue.c
SRC += pbx/as_beep.c
SRC += pbx/as_button.c
SRC += pbx/as_switch.c
SRC += pbx/as_onoffhook.c
SRC += pbx/as_sn138.c
SRC += uart/as_uart.c 
SRC += utils/as_thread.c 
SRC += utils/as_utils.c 
SRC += version/as_version.c 
SRC += voice/as_g711_law.c 
SRC += voice/as_dtmf_gen.c 
SRC += recorder/as_taper.c 
SRC += lcd/as_lcd.c 
SRC += serialno/as_serialno.c  
SRC += flash/as_flash.c 
	
TEST_CODE_SUBDIRS = test
	
TEST_SRC += test/test_utils.c 
TEST_SRC += test/test_beep.c 
TEST_SRC += test/test_button.c 
TEST_SRC += test/test_watchdog.c 
TEST_SRC += test/test_serialno.c 
TEST_SRC += test/test_flash.c 
TEST_SRC += test/test_switch.c 
TEST_SRC += test/test_onoffhook.c 
TEST_SRC += test/test_sn138.c 
LOCAL_CLEAN:=clear

LOCAL_ALL:


$(LOCAL_CLEAN):
	
TARGET_LIB:=libaslib
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

LOCAL_test += test_log
LOCAL_test += test_beep 
LOCAL_test += test_button 
LOCAL_test += test_watchdog 
LOCAL_test += test_serialno 
LOCAL_test += test_flash 
LOCAL_test += test_switch
LOCAL_test += test_onoffhook
LOCAL_test += test_sn138

include $(TOP_DIR)/Makefile.post

TEST_LOG_OBJS = $(TEST_OBJ_DIR)/test_utils.o 
TEST_BEEP_OBJS = $(TEST_OBJ_DIR)/test_beep.o
TEST_BUTTON_OBJS = $(TEST_OBJ_DIR)/test_button.o
TEST_WATCHDOG_OBJS = $(TEST_OBJ_DIR)/test_watchdog.o
TEST_SERIALNO_OBJS = $(TEST_OBJ_DIR)/test_serialno.o
TEST_FLASH_OBJS = $(TEST_OBJ_DIR)/test_flash.o
TEST_SWITCH_OBJS = $(TEST_OBJ_DIR)/test_switch.o
TEST_ONOFFHOOK_OBJS = $(TEST_OBJ_DIR)/test_onoffhook.o
TEST_SN138_OBJS = $(TEST_OBJ_DIR)/test_sn138.o

TEST_LDFLAGS := -L$(BIN_DIR) -laslib -lpthread -lm

test_log: $(TEST_LOG_OBJS)
	$(CC) $(CFLAGS) -o  test_log $(TEST_LOG_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_log
	$(MOVE) test_log $(TEST_BIN_DIR)
	
test_beep:$(TEST_BEEP_OBJS) 
	$(CC) $(CFLAGS) -o test_beep $(TEST_BEEP_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_beep
	$(MOVE) $@ $(TEST_BIN_DIR) 

test_button:$(TEST_BUTTON_OBJS) 
	$(CC) $(CFLAGS) -o test_button $(TEST_BUTTON_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_button
	$(MOVE) $@ $(TEST_BIN_DIR) 
	
test_watchdog:$(TEST_WATCHDOG_OBJS) 
	$(CC) $(CFLAGS) -o test_watchdog $(TEST_WATCHDOG_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_watchdog
	$(MOVE) $@ $(TEST_BIN_DIR) 
	
test_serialno:$(TEST_SERIALNO_OBJS) 
	$(CC) $(CFLAGS) -o test_serialno $(TEST_SERIALNO_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_serialno
	$(MOVE) $@ $(TEST_BIN_DIR) 
	
test_flash:$(TEST_FLASH_OBJS) 
	$(CC) $(CFLAGS) -o test_flash $(TEST_FLASH_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_flash
	$(MOVE) $@ $(TEST_BIN_DIR)
	
test_switch:$(TEST_SWITCH_OBJS) 
	$(CC) $(CFLAGS) -o test_switch $(TEST_SWITCH_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_switch
	$(MOVE) $@ $(TEST_BIN_DIR)
	
test_onoffhook:$(TEST_ONOFFHOOK_OBJS) 
	$(CC) $(CFLAGS) -o test_onoffhook $(TEST_ONOFFHOOK_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_onoffhook
	$(MOVE) $@ $(TEST_BIN_DIR)

test_sn138:$(TEST_SN138_OBJS) 
	$(CC) $(CFLAGS) -o test_sn138 $(TEST_SN138_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_sn138
	$(MOVE) $@ $(TEST_BIN_DIR)
	
ifeq ($(WITH_93LC66),no)
else
test_write_93lc66:$(TEST_WRITE_93LC66_OBJS) 
	$(CC) $(CFLAGS) -o test_write_93lc66 $(TEST_WRITE_93LC66_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_write_93lc66
	$(MOVE) $@ $(TEST_BIN_DIR)
	
test_read_93lc66:$(TEST_READ_93LC66_OBJS) 
	$(CC) $(CFLAGS) -o test_read_93lc66 $(TEST_READ_93LC66_OBJS) $(TEST_LDFLAGS)
	$(STRIP) test_read_93lc66
	$(MOVE) $@ $(TEST_BIN_DIR)
endif
install: all
	$(MKDIR) $(INCLUDED)
	$(MKDIR) $(LIBDIR)
	$(INSTALL) -m 644 pwdb/pwdb_public.h $(INCLUDED)
	$(INSTALL) -m 644 ./pwdb_map.h $(INCLUDED)
	$(INSTALL) -m 644 unix/public.h $(INCLUDED)/pwdb_unix.h
	$(INSTALL) -m 644 shadow/public.h $(INCLUDED)/pwdb_shadow.h
	$(INSTALL) -m 644 common/public.h $(INCLUDED)/pwdb_common.h
	$(INSTALL) -m 644 radius/public.h $(INCLUDED)/pwdb_radius.h
	$(INSTALL) -m 644 radius.h $(INCLUDED)/radius.h
	$(INSTALL) -m 644 _pwdb_macros.h $(INCLUDED)/_pwdb_macros.h
	$(COPY) $(BIN_DIR)/libpwdb.so $(LIBDIR)
	ln -sf $(LIBDIR)/libpwdb.so $(LIBDIR)/libpwdb.so.0.60 
#	$(INSTALL) -m 755 $(BIN_DIR)/libpwd* $(LIBDIR)
#	$(LDCONFIG)
#	ln -sf $(LIBFILENAME) $(LIBDIR)/$(LIBDYNAME)
#	$(INSTALL) -m 644 $(LIBSTATIC) $(LIBDIR)
