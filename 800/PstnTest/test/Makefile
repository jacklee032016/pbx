#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
# AS800 PstnTest
#
# Revision 1.2  2006/03/13 09:34:48  wangwei
# no message
#
# Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
# PSTN test library
#
# Revision 1.3  2005/04/26 06:06:10  lizhijie
# *** empty log message ***
#
# Revision 1.2  2005/04/20 03:06:02  lizhijie
# no message
#
# Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
# new drivers for all devices
#
# Revision 1.13  2004/12/20 03:18:15  lizhijie
# Add DSP library for FSK detect/generate, DTMF detect and other test code
#
# Revision 1.12  2004/12/15 07:33:05  lizhijie
# recommit
#
# Revision 1.11  2004/12/14 12:48:50  lizhijie
# support building header files in the architecture platform
#
# Revision 1.10  2004/12/11 06:29:24  lizhijie
# *** empty log message ***
#
# Revision 1.9  2004/12/11 06:05:13  lizhijie
# *** empty log message ***
#
# Revision 1.8  2004/12/11 06:04:30  lizhijie
# merge into CVS log
#
# Revision 1.7  2004/12/11 05:05:30  eagle
# add test_gain.c
#
# Revision 1.6  2004/12/09 07:46:22  lizhijie
# no message
#
# Revision 1.5  2004/11/29 08:25:03  eagle
# 2229 by chenchaoxin
#
# Revision 1.4  2004/11/25 08:08:16  lizhijie
# add echo and misc directory in delete list
#
# Revision 1.3  2004/11/25 07:32:02  lizhijie
# modify for the sub-directories
#
# Revision 1.2  2004/11/22 01:54:04  lizhijie
# add some user module into CVS
#
# Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
# driver for assist telephone cards Tiger320-Si3210/3050
#
# $Revision: 1.1.1.1 $
#

include $(ROOT_DIR)/Makefile.pre

KERNEL=no

CODE_SUBDIRS = 


DELETE_LIST += test_delay_switch
DELETE_LIST += test_tones
DELETE_LIST += test_ring
DELETE_LIST += test_gsm
DELETE_LIST += test_rw
DELETE_LIST += test_chan_states
DELETE_LIST += test_ring_param
DELETE_LIST += test_buffer_info
DELETE_LIST += test_gain
DELETE_LIST += test_type

TEST_LIB=$(BIN_DIR)/libpstn_test.so


############ Program 1
test_tones:test_tones.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

test_gsm:test_gsmplay.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -lgsm_$(ARCH)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

test_chan_states:test_chan_states.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

test_ring_param:test_ring_param.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

test_buffer_info:test_buffer_info.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

test_ring:test_ring.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

test_delay_switch:test_delay_switch.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

dtmf_test:
	make -C dtmf 

fsk_test:
	make -C fsk

fxo_test:
	make -C fxo

misc_test:
	make -C misc

echo_test:
	make -C echo

dsp_test:
	make -C dsp
	
LDFLAGS+=-L$(BIN_DIR) -lpstn_test -lpthread -lm 

LOCAL_ALL=program

$(LOCAL_ALL):$(OBJS) test_tones test_ring \
	fxo_test \
	dtmf_test \
	misc_test \
	fsk_test \
	dsp_test \
	echo_test test_chan_states test_ring_param \
	test_buffer_info \
	test_type \
	test_delay_switch

#��test_gsm 

LOCAL_CLEAN:=clear

$(LOCAL_CLEAN):
	rm -f *.o *.~ *.*~
	make -C fxo clean
	make -C dtmf clean
	make -C echo clean
	make -C misc clean
	make -C dsp clean
	for n in $(DELETE_LIST);do \
		if test -f "$$n"; then \
			$(RM) "$$n";\
	  else :; fi; \
	done

include $(ROOT_DIR)/Makefile.post

