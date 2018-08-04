#
# $Id: Makefile,v 1.5 2007/03/26 08:53:00 lizhijie Exp $
#

TOP_DIR:=$(shell pwd)/..

include $(TOP_DIR)/Makefile.pre

CODE_SUBDIRS = \
			devs \
			recorder \
			uart \
			utils \
			version \
			voice 

#			serialno \
#			flash  \
#			pbx \
#			lcd \

################################################
####   source file 
################################################
#		serialno/as_serialno.c \
#		lcd/as_lcd.c 
SRC += \
		devs/as_flash.c \
		devs/as_beep.c \
		devs/as_button.c \
		devs/as_watchdog.c \
		recorder/as_record_offhook.c \
		recorder/as_record_working.c \
		uart/as_msg_queue.c \
		uart/as_pbx_cmd.c \
		uart/as_uart.c \
		utils/as_thread.c \
		utils/as_utils.c \
		utils/as_timer.c \
		version/as_version.c 
		
ifeq ($(FLASH),YES)
else
	SRC += \
		voice/as_g711_law.c \
		voice/as_dtmf_gen.c 
endif


# m93lc66用于向交换芯片使用的EEPROM写入数据，在AS600上没有此芯片
ifeq ($(WITH_93LC66),yes)
	CODE_SUBDIRS += m93lc66
	SRC += m93lc66/as_93lc66.c
	
	TEST_SRC += test/test_write_93lc66.c 
	TEST_SRC += test/test_read_93lc66.c
	LOCAL_test += test_write_93lc66 
	LOCAL_test += test_read_93lc66
	TEST_WRITE_93LC66_OBJS = $(TEST_OBJ_DIR)/test_write_93lc66.o
	TEST_READ_93LC66_OBJS = $(TEST_OBJ_DIR)/test_read_93lc66.o
endif


TARGET_LIB:=libaslib
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

include $(ROOT_DIR)/Makefile.post

# LDFLAGS += -L$(TOP_DIR)/bin/lib/arm -L$(BIN_DIR) -L$(LIBS_DIR)/libs/bin/$(ARCH) \
#           -L$(TOP_DIR)/shared_lib/libs/bin/$(ARCH)

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
#	tar cvzf  p.tar.gz $@
#	rm $@
	$(MOVE) $@ $(BIN_DIR)
	# tar cvzf p.tar.gz $(BIN_DIR)/$@tar cvzf p.tar.gz $(BIN_DIR)/$@
