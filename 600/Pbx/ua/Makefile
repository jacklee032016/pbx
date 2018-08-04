#
# $Id: Makefile,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
ASSIST_RELEASES_NAME=assist_ippbx_$(BUILDTIME).tar 

PACKAGES += SDP2 UTIL PTHREAD LIBXML

CODE_SUBDIRS = assist #op assist/dsp

# name of all files to be compiled (don't include header files)
SRC += \
	Authenticate.cxx\
	Contact.cxx \
	DeviceThread.cxx \
	DigitCollector.cxx \
	DigitTimeout.cxx \
	FeatureThread.cxx \
	Registration.cxx \
	RegistrationManager.cxx \
	ResGwDevice.cxx \
	RsvpHandler.cxx \
	RtpThread.cxx \
	SdpHandler.cxx\
	SubscribeManager.cxx \
	UaBuilder.cxx \
	UaCallContainer.cxx \
	UaCallInfo.cxx \
	UaConfiguration.cxx \
	UaDevice.cxx \
	UaDeviceEvent.cxx \
	UaDigitTimerEvent.cxx \
	UaHardwareEvent.cxx \
	UaOperator.cxx \
	UaStateMachine.cxx \
	UaTimerEvent.cxx \
	UserAgent.cxx \
	ua.cxx \
	assist/AssistDevice.cxx \
	assist/AssistDeviceInfo.cxx \
	assist/AssistPbxDevice.cxx \
	assist/AssistNullDevice.cxx \
	assist/AssistInvalidDevice.cxx \
	assist/AssistRtpTransmitter.cxx \
	assist/AssistRtpTransVad.cxx \
	assist/AssistPbxRtpThread.cxx \
	assist/RecordThread.cxx \
	assist/SimulateDB.cxx
	
#	assist/AssistCheckHardware.cxx \
#	assist/AssistDeviceContainer.cxx 
#	NullHwDevice.cxx \

#	SimpleExtDevice.cxx\
#	PlayQueue.cxx \
#	Recorder.cxx \
#	UaCommandLine.cxx \
#	LoadGenMonitor.cxx \
#	LoadGenThread.cxx \
#	CJTime.cxx \

INCLUDE_DIR=  \
	-I$(TOP_DIR)/include \
	$(DRIVER_INC_DIR) \
	$(UTILS_INC_DIR) \
	$(SDP_INC_DIR) \
	$(RTP_INC_DIR) \
	$(SIPSTACK_INC_DIR) \
	$(MEDIA_INC_DIR) \
	$(BASE_INC_DIR) \
	$(SIPUA_INC_DIR) \
	$(UA_INC_DIR) \
	$(UA_STATE_INC_DIR) \
	$(UA_OP_INC_DIR) \
	$(ASSIST_INC_DIR)
	

C_CFLAGS+= -D_REENTRANT -DUSE_PTHREADS -DVOCAL_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM -DNO_POLICY 


LOCAL_ALL=pbx


# TARGET_LIB = $(BIN_DIR)/libgua

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -L$(VOIPS_DIR)/binary -L$(BIN_DIR)/lib -L$(LIBS_DIR)/libs/bin/$(ARCH)

LIBS += -lvoip_proxyBase -lvoip_uastate -lvoip_uaop  
LIBS += -lvoip_sipstack -lvoip_sdp -lvoip_rtp -lvoip_util
LIBS += -lvoip_assist -laslib
LIBS += -lpthread -lm -lstdc++  

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)/usr/bin
#$(BIN_DIR)
UA_CDR_DIR += $(OBJ_DIR)/assist
UA_CDR_OBJS = $(UA_CDR_DIR)/SimulateDB.o $(UA_CDR_DIR)/RecordThread.o \
	$(OBJ_DIR)/UaConfiguration.o $(OBJ_DIR)/UserAgent.o
cleancdr:
	rm -r -f $(UA_CDR_OBJS)
