#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:23:05 lizhijie Exp $
#

TOP_DIR:=$(shell pwd)/..

include $(TOP_DIR)/Makefile.pre

CODE_SUBDIRS = gsm use control dtmf utils  channel tonegen fsk busydetect misc  # law io ioctl file 

################################################
####   source file 
################################################

GSM_SRC += gsm/add.c \
	gsm/code.c \
	gsm/debug.c \
	gsm/decode.c \
	gsm/long_term.c \
	gsm/lpc.c \
	gsm/preprocess.c \
	gsm/rpe.c \
	gsm/gsm_destroy.c \
	gsm/gsm_decode.c \
	gsm/gsm_encode.c \
	gsm/gsm_explode.c \
	gsm/gsm_implode.c \
	gsm/gsm_create.c \
	gsm/gsm_print.c \
	gsm/gsm_option.c \
	gsm/short_term.c \
	gsm/table.c
	
	
SRC += \
	$(GSM_SRC) \
	control/as_lib_tonezone.c \
	control/as_tonezone_data.c \
	control/as_lib_ring_param.c \
	use/as_lib_gsm.c \
	use/as_lib_ring.c \
	use/as_lib_tones.c \
	use/as_lib_law.c \
	use/as_lib_hard_dtmf.c \
	dtmf/as_lib_dtmf_detect.c \
	dtmf/as_lib_dtmf_generator.c \
	utils/as_fifo.c \
	utils/as_list.c \
	utils/as_thread.c \
	as_lib_utils.c \
	channel/as_lib_buf.c \
	tonegen/as_dsp_utils.c \
	tonegen/as_dsp_tone_gen.c \
	tonegen/as_dsp_fsk_generator.c \
	tonegen/as_dsp_dtmf_generator.c \
	tonegen/as_dsp_gain.c \
	fsk/as_dsp_fsk_detect.c \
	fsk/as_dsp_demodule.c \
	fsk/as_dsp_filtros.c \
	busydetect/as_busy_detect.c \
	misc/assist_misc_93lc66.c \
	misc/assist_misc_gpio_ctl.c \
	misc/assist_misc_watchdog.c
	
#	utils/as_network.c \
#	utils/as_network.c \
#	utils/as_log.c \
	
C_FLAGS+= -Igsm

TARGET_LIB:=libpstn_test
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

include $(ROOT_DIR)/Makefile.post

# LDFLAGS += -L$(TOP_DIR)/bin/lib/arm -L$(BIN_DIR) -L$(LIBS_DIR)/libs/bin/$(ARCH) \
#           -L$(TOP_DIR)/shared_lib/libs/bin/$(ARCH)

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	tar cvzf  p.tar.gz $@
	rm $@
	# $(MOVE) $@ $(BIN_DIR)
	# tar cvzf p.tar.gz $(BIN_DIR)/$@tar cvzf p.tar.gz $(BIN_DIR)/$@
