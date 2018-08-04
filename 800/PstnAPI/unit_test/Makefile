#
# $Author: lizhijie $
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
#

include $(ROOT_DIR)/Makefile.pre

KERNEL=no

CODE_SUBDIRS = \
		device device/car device/fxo \
		dsp dsp/busy dsp/fsk dsp/ntt dsp/dp

#fxo  

DELETE_LIST += states
DELETE_LIST += ring
DELETE_LIST += tones
DELETE_LIST += rx_dtmf

DELETE_LIST += car_fxo 
DELETE_LIST += car_fxs 
DELETE_LIST += caller
DELETE_LIST += callee

DELETE_LIST += flash
DELETE_LIST += onhook
DELETE_LIST += event

DELETE_LIST += fxo_line
DELETE_LIST += fxo_callee
DELETE_LIST += fxo_call

DELETE_LIST += dtmf_dsp_detect
DELETE_LIST += dsp_tones
DELETE_LIST += dsp_dtmf_gen
DELETE_LIST += rx_dp
DELETE_LIST += fsk_gen
DELETE_LIST += fsk_detect
DELETE_LIST += fsk_file

DELETE_LIST += busy
DELETE_LIST += busy_fxo
DELETE_LIST += tone_make
DELETE_LIST += progress
DELETE_LIST += progress_fxo


DELETE_LIST += ntt_cid
DELETE_LIST += ntt_bcid
DELETE_LIST += ntt_rx
DELETE_LIST += ntt_detect 
DELETE_LIST += ntt_dem 
DELETE_LIST += fsk_crc 

DELETE_LIST += fxo_dp

DELETE_LIST += delay_chan_ctl
DELETE_LIST += hook_status

TEST_LIB=$(BIN_DIR)/libpstn.so

CFLAGS += -I$(ROOT_DIR)/lib/device 

############ Program 1
states:device/test_device_status.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device

ring:device/test_ring.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device

tones:device/test_tones.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device 

flash:device/test_flash.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device 
	
onhook:device/test_onhook.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device 

event:device/test_event.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device 


rx_dtmf:device/test_rx_dtmf_4_fxs.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device 

rx_dp:device/test_rx_dp.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device 

####  test for CAR Signal (FXO and FXS)
car_fxo:device/car/test_car_fxo.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/car 
car_fxs:device/car/test_car_fxs.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/car  

####  FXO test 
fxo_ccid:device/fxo/test_fxo_rx_chinese_cid.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
fxo_cid:device/fxo/test_fxo_rx_data.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
fxo_ncid:device/fxo/test_fxo_rx_cid.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
fxo_line:device/fxo/test_fxo_line.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
fxo_dp:device/fxo/test_fxo_dp.c    
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
fxo_call:device/fxo/test_fxo_calling.c    
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
fxo_callee:device/fxo/test_fxo_callee.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
caller:device/fxo/fxo_caller.c    
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo 
callee:device/fxo/fxo_callee.c    
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device/fxo



delay_chan_ctl:device/test_delay_chan_ctl.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device
	
hook_status:device/test_hook_status.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/device


########################################################
#################### DSP test 
########################################################
dtmf_decoder:dsp/test_dtmf_decoder.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/dtmf 
	
dtmf_dsp_detect:dsp/test_dtmf_detect.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/dtmf 
	
dsp_dtmf_gen:dsp/test_dsp_dtmf_gen.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/dtmf 

dsp_tones:dsp/test_dsp_tones.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/tone 


######## FSK test cases
fsk_gen:dsp/fsk/test_fsk_caller_id.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/fsk 
china_cid:dsp/fsk/test_fsk_chinese_callerid_buf.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/fsk 
fsk_detect:dsp/fsk/test_fsk_detect.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/fsk
fsk_file:dsp/fsk/test_fsk_file.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/fsk 
	
fsk_crc:dsp/fsk/test_fsk_crc.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/fsk 
ntt_cid:dsp/fsk/test_fsk_ntt_callerid.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/fsk 
ntt_bcid:dsp/fsk/test_fsk_ntt_callerid_buf.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/fsk 


ntt_dem:dsp/ntt/test_ntt_detect_buf.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp 
ntt_detect:dsp/ntt/test_ntt_detect.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp 
ntt_data:dsp/ntt/test_catch_modem_data.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp 



######  BT(Busy Tone) detect 
busy:dsp/busy/test_busy_detect.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/busy 

busy_fxo:dsp/busy/test_busy_fxo.c   
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/busy 

tone_make:dsp/progress/test_tones_create.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/progress  
progress:dsp/progress/test_progress_detect.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/progress  
progress_fxo:dsp/progress/test_progress_fxo.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/dsp/progress  
	

dtmf_test:
	make -C dtmf 

	
C_FLAGS+= 
	
LDFLAGS+=-L$(BIN_DIR) -lpstn -lpthread -lm 

LOCAL_ALL=program

$(LOCAL_ALL):$(OBJS) states ring tones rx_dtmf flash \
	car_fxo car_fxs \
	onhook event \
	fxo_callee fxo_line fxo_dp\
	fxo_ncid fxo_cid fxo_ccid \
	fxo_call caller callee\
	rx_dp \
	fsk_gen china_cid fsk_detect fsk_file \
	ntt_dem ntt_detect ntt_data \
	ntt_cid ntt_bcid fsk_crc \
	dtmf_dsp_detect dtmf_decoder \
	dsp_tones dsp_dtmf_gen \
	busy busy_fxo \
	tone_make progress progress_fxo \
	delay_chan_ctl hook_status

LOCAL_CLEAN:=clear

$(LOCAL_CLEAN):
	rm -f *.o *.~ *.*~
#	make -C fxo clean
#	make -C dtmf clean
#	make -C echo clean
#	make -C misc clean
#	make -C dsp clean
	for n in $(DELETE_LIST);do \
		if test -f "$$n"; then \
			$(RM) "$$n";\
	  else :; fi; \
	done

include $(ROOT_DIR)/Makefile.post

