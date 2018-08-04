#$Id: Makefile,v 1.2 2007/09/08 18:21:40 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

CODE_SUBDIRS = dsp slic uart pcm test cdr  

# name of all files to be compiled (don't include header files)
SRC += \
	assist_init.c \
	dsp/assist_dsp_gain.c \
	dsp/assist_dsp.c \
	slic/assist_slic_state.c \
	slic/assist_slic_tone.c \
	slic/assist_slic_dtmf.c \
	slic/assist_slic_ring.c \
	pcm/assist_pcm_state.c
	
#	dsp/assist_dtmf_detect.c \
#	dsp/assist_dsp_law.c \

#	assist_utils.c \
#	dsp/as_dsp_tone_gen.c \
#	dsp/as_dsp_fsk_filtros.c \
#	dsp/as_dsp_fsk_demodule.c \
#	dsp/as_dsp_fsk_detect.c	\
	

INCLUDE_DIR=  \
	-I$(ROOT_DIR)/include \
	$(DRIVER_INC_DIR) \
	$(ASSIST_INC_DIR) 
	

C_CFLAGS+=  -fPIC 
		 

#		pcm/assist_pcm_tone.c \
#		pcm/assist_pcm_dtmf.c 

LOCAL_ALL=test_dsp_dtmf_detect_$(ARCH)

TARGET_LIB = libvoip_assist

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -L$(VOIPS_DIR)/binary -L$(BIN_DIR)/lib  -lpthread

test_dsp_dtmf_detect_$(ARCH):$(TARGET_LIB)
	$(CC) $(C_CFLAGS) -o test/$@ test/test_dsp_dtmf_detect.c $(LDFLAGS) -lvoip_assist -lm
	$(STRIP) test/$@
CDRSUBDIRS = cdr
all: 
	for i in $(CDRSUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done

MKINSTALL = make ARCH=$(ARCH) install	
install:
	cd cdr && $(MKINSTALL)
