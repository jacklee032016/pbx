#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
# AS800 PstnTest
#
# Revision 1.2  2006/04/27 05:43:28  wangwei
# no message
#
# Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
# PSTN test library
#
# Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
# new drivers for all devices
#
# Revision 1.3  2004/12/14 12:48:50  lizhijie
# support building header files in the architecture platform
#
# Revision 1.2  2004/12/11 05:44:20  lizhijie
# some comile warning
#
# Revision 1.1  2004/11/25 07:36:13  lizhijie
# create the sub-directories for different test
#
# $Revision: 1.1.1.1 $
#

include $(ROOT_DIR)/Makefile.pre

KERNEL=no


CODE_SUBDIRS = 

DELETE_LIST:= register_stat 
 

LDFLAGS+=-L$(BIN_DIR) -lpstn_test -lpthread -lm 

LOCAL_LAST=program

$(LOCAL_LAST):$(OBJS) register_stat
$(LOCAL_LAST):$(OBJS) led_status
$(LOCAL_LAST):$(OBJS) delay_chan_ctl
$(LOCAL_LAST):$(OBJS) factory_button
$(LOCAL_LAST):$(OBJS) hook_status
$(LOCAL_LAST):$(OBJS) led_line
$(LOCAL_LAST):$(OBJS) led_pppoe1
$(LOCAL_LAST):$(OBJS) led_pppoe2
$(LOCAL_LAST):$(OBJS) beep
$(LOCAL_LAST):$(OBJS) led_voip
$(LOCAL_LAST):$(OBJS) read_93lc66
$(LOCAL_LAST):$(OBJS) watchdog
$(LOCAL_LAST):$(OBJS) write_93lc66


############ Program 1
register_stat:register_stat.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

led_status:test_led_status.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
delay_chan_ctl:test_delay_chan_ctl.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
factory_button:test_factory_button.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
hook_status:test_hook_status.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
led_line:test_led_line.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
led_pppoe1:test_led_pppoe1.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
led_pppoe2:test_led_pppoe2.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
beep:test_beep.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
led_voip:test_led_voip.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
read_93lc66:test_read_93lc66.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
watchdog:test_watchdog.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
write_93lc66:test_write_93lc66.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)
	
LOCAL_CLEAN:=clear

$(LOCAL_CLEAN):
	rm -f *.o *.~ *.*~
	for n in $(DELETE_LIST);do \
		if test -f "$$n"; then \
			$(RM) "$$n";\
	  else :; fi; \
	done

include $(ROOT_DIR)/Makefile.post
