#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
#


VOIP_DIR:=$(shell pwd)
#ROOT_DIR = $(VOIP_DIR)

TOP_DIR:=$(shell pwd)/../..

include $(TOP_DIR)/Makefile.pre

CODE_SUBDIRS = 	\
	device event state \
	op op/caller op/callee op/incall \
	ua  

# name of all files to be compiled (don't include header files)

################################################
####   source file 
################################################
DEVICE_SRC = \
	device/VoipDeviceMgr.cxx \
	device/VoipCtrlDevice.cxx \
	device/VoipAudioChannel.cxx \
	device/VoipEndPoint.cxx 

	
#	\
#	device/VoipAdapterDevice.cxx 
	
EVENT_SRC = \
	event/SipEvent.cxx \

STATE_SRC = \
	state/VoipStateBuilder.cxx 
	
SIP_OP_SRC += \
		op/VoipOperator.cxx \
		op/VoipOpBuilder.cxx \
        op/callee/VoipOpMsgInviteIdle.cxx \
        op/callee/VoipOpMsgInviteAuthenIdle.cxx \
        op/callee/VoipOpMsgInviteAuthorIdle.cxx \
        op/callee/VoipOpMsgCancelByeRinging.cxx \
        op/callee/VoipOpMsgReInviteRinging.cxx \
        op/callee/VoipOpCallReleaseRinging.cxx \
        op/callee/VoipOpCallProceedingRinging.cxx \
        op/callee/VoipOpCallConnectRinging.cxx \
        op/callee/VoipOpTimeoutRinging.cxx \
        op/caller/VoipOpCallSetupIdle.cxx \
        op/caller/VoipOpCallReleaseTrying.cxx \
        op/caller/VoipOpMsgStatusCerrAuthenTrying.cxx \
        op/caller/VoipOpMsgStatusCerrBusyTrying.cxx \
        op/caller/VoipOpMsgStatusErrorTrying.cxx \
        op/caller/VoipOpMsgStatusInfoTrying.cxx \
        op/caller/VoipOpMsgStatusRedirectTrying.cxx \
        op/caller/VoipOpMsgStatusSuccessTrying.cxx \
        op/caller/VoipOpStopRingbackTrying.cxx \
        op/caller/VoipOpMsgCancelOKTrying.cxx \
        op/incall/VoipOpCallDisconnectInCall.cxx \
        op/incall/VoipOpFwdDigitInCall.cxx \
        op/incall/VoipOpMsgAckInCall.cxx \
        op/incall/VoipOpMsgByeInCall.cxx \
        op/incall/VoipOpMsgConfTargetOkInCall.cxx \
        op/incall/VoipOpMsgReInviteInCall.cxx \
        op/incall/VoipOpRtpTimeoutInCall.cxx 

#        op/incall/VoipOpStartCallWaitingInCall.cxx \

SIP_UA_SRC += \
	ua/BaseAuthentication.cxx \
	ua/Authenticate.cxx\
	ua/CommonFunctions.cxx \
	ua/Contact.cxx \
	ua/SubscribeThread.cxx \
	ua/SubscribeManager.cxx \
	ua/Registration.cxx \
	ua/RegistrationManager.cxx \
	ua/RsvpHandler.cxx \
	ua/SdpHandler.cxx\
	ua/UaBuilder.cxx \
	ua/SipContext.cxx 

SRC += \
	$(DEVICE_SRC) \
	$(EVENT_SRC) \
	$(STATE_SRC) \
	$(SIP_UA_SRC) \
	$(SIP_OP_SRC) \
	voip.cxx 
	
#BUILD = ../build
AUTILS_DIR = $(VOIP_DIR)/libs/include/util
AUTILS_INC_DIR+=-I$(BUILD) \
	-I$(AUTILS_DIR) \
	-I$(AUTILS_DIR)/threads \
	-I$(AUTILS_DIR)/logging \
	-I$(AUTILS_DIR)/crypto \
	-I$(AUTILS_DIR)/statistics \
	-I$(AUTILS_DIR)/snmp \
	-I$(AUTILS_DIR)/signals \
	-I$(AUTILS_DIR)/behavior \
	-I$(AUTILS_DIR)/io \
	-I$(AUTILS_DIR)/services \
	-I$(AUTILS_DIR)/transport \
	-I$(AUTILS_DIR)/config \
	-I$(AUTILS_DIR)/dnssrv \
	-I$(AUTILS_DIR)/deprecated \
	-I$(AUTILS_DIR)/adt 

MY_INC_DIR = \
	-I$(VOIP_DIR)/ \
	-I$(VOIP_DIR)/device \
	-I$(VOIP_DIR)/event \
	-I$(VOIP_DIR)/state \
	-I$(VOIP_DIR)/ua \
	-I$(VOIP_DIR)/op \
	-I$(VOIP_DIR)/libs \
	-I$(VOIP_DIR)/include \
	-I$(VOIP_DIR)/stacks/build \
	-I$(VOIP_DIR)/stacks/sipstack \
	-I$(VOIP_DIR)/stacks/rtp \
	-I$(VOIP_DIR)/stacks/sdp \
	-I$(VOIP_DIR)/stacks/sdp/sdp2


INCLUDE_DIR=  \
	-I$(TOP_DIR)/include \
	$(DRIVER_INC_DIR) \
	$(UTILS_INC_DIR) \
	$(PBX_INC_DIR) \
	$(MY_INC_DIR)

C_CFLAGS+= -D_REENTRANT -DASSIST_USE_POSIX_THREADS -DVOCAL_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM -DNO_POLICY \
		 $(COMPILE_OPTIONS) $(CFLAGS) 

#PBX_UTIL_OBJS = $(VPBX_DIR)/Linux.obj.$(ARCH)/util/*.o	 		 

TARGET_LIB:=ASVPBXPlugInVoip
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
TARGET_LIB_PROG:=voip

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -L$(TOP_DIR)/bin/lib/arm -L$(BIN_DIR) -L$(VOIP_DIR)/libs/bin/$(ARCH) 

LIBS += -lsipstack -lsdp -lrtp
# LIBS += -lvpbxutil -lpthread -lm -lstdc++  

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	tar cvzf  p.tar.gz $@
	rm $@
	# $(MOVE) $@ $(BIN_DIR)
	# tar cvzf p.tar.gz $(BIN_DIR)/$@tar cvzf p.tar.gz $(BIN_DIR)/$@
