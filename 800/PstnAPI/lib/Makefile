#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
# $Author: lizhijie $
#

TOP_DIR:=$(shell pwd)/..

include $(TOP_DIR)/Makefile.pre

CODE_SUBDIRS = device \
		device/fxo device/car \
		dsp dsp/dtmf dsp/tone dsp/fsk \
		dsp/busy dsp/g711 dsp/progress \
		utils dsp/dp

################################################
####   source file 
################################################
SRC += \
	device/assist_pstn_state.c \
	device/assist_pstn_tone.c \
	device/assist_pstn_dtmf.c \
	device/assist_pstn_ring.c \
	device/assist_misc_gpio_ctl.c \
	dsp/dtmf/as_dtmf_detect.c \
	dsp/dtmf/as_dtmf_decoder.c \
	dsp/fsk/as_fsk_filtros.c \
	dsp/fsk/as_fsk_demodule.c \
	dsp/fsk/as_fsk_demodule_buf_both.c \
	dsp/fsk/as_fsk_decoder.c \
	dsp/fsk/as_fsk_encoder.c \
	dsp/fsk/as_fsk_encoder_buf.c \
	dsp/fsk/as_fsk_crc.c \
	dsp/fsk/as_fsk.c \
	dsp/dp/assist_fxo_dp.c \
	dsp/tone/as_tone_utils.c \
	dsp/tone/as_tone.c \
	dsp/busy/as_busy_detect.c \
	dsp/progress/as_progress_detect.c \
	dsp/g711/g711.c \
	dsp/g711/dtmf_u.c \
	dsp/as_dsp.c \
	utils/as_msg.c \
	utils/as_thread.c 
#	device/assist_pstn_switch.c 
	
#	dsp/fsk/as_fsk_demodule_buf.c \
#	dsp/dtmf/as_dtmf_generator.c \
#	dsp/as_dsp_law.c \

TARGET_LIB:=libpstn
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

include $(ROOT_DIR)/Makefile.post

C_FLAGS+= \
	-I. -I$(ROOT_DIR)/lib/device \
	-I$(ROOT_DIR)/lib/dsp \
	-I$(ROOT_DIR)/lib/dsp/dtmf \
	-I$(ROOT_DIR)/lib/dsp/fsk \
	-I$(ROOT_DIR)/lib/dsp/tone \
	-I$(ROOT_DIR)/lib/dsp/g711 \
	-I$(ROOT_DIR)/lib/dsp/progress \
	-I$(ROOT_DIR)/lib/dsp/busy 
	

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	rm $@
	# $(MOVE) $@ $(BIN_DIR)

