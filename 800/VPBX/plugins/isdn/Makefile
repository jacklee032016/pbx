#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
#

ISDN_NT=yes

ISDN_DIR:=$(shell pwd)

TOP_DIR:=$(shell pwd)/../..

include $(TOP_DIR)/Makefile.pre


CODE_SUBDIRS = 	\
	device event state op \
	msg msg/IEs msg/msg \
	driver/ask  driver/asnt \
	driver/asnt/utils \
	driver/asnt/layer3 
	

# name of all files to be compiled (don't include header files)

################################################
####   source file 
################################################
DEVICE_SRC += \
	device/IsdnDeviceMgr.cxx \
	device/IsdnMgrThread.cxx \
	device/IsdnCtrlDevice.cxx \
	device/IsdnTeDevice.cxx \
	device/IsdnNtDevice.cxx \
	device/IsdnAudioChannel.cxx \
	device/IsdnEndPoint.cxx 

	
EVENT_SRC += \
	event/IsdnEvent.cxx
	
OP_SRC += \
	op/IsdnOperator.cxx \
	op/IsdnOpBuilder.cxx \
	op/IsdnOpNewCC.cxx \
	op/IsdnOpSetupMsg.cxx \
	op/IsdnOpProceedingCall.cxx \
	op/IsdnOpAlertingCall.cxx \
	op/IsdnOpConnectCall.cxx \
	op/IsdnOpConnectAckMsg.cxx \
	op/IsdnOpDisconnectMsg.cxx \
	op/IsdnOpDisconnectCall.cxx \
	op/IsdnOpSetupCall.cxx \
	op/IsdnOpProceedingMsg.cxx \
	op/IsdnOpAlertingMsg.cxx \
	op/IsdnOpConnectMsg.cxx \
	op/IsdnOpReleaseMsg.cxx \
	op/IsdnOpReleaseCall.cxx \
	op/IsdnOpReleaseCompleteMsg.cxx \
	op/IsdnOpReleaseCompleteCall.cxx \
	op/IsdnOpReleaseCRMsg.cxx 

OP_SRC2 += \
	op/ep/IsdnOpSetupSignal.cxx 
		
MSG_SRC += \
	msg/IsdnParserMode.cxx \
	msg/IsdnIE.cxx \
	msg/IEs/IeBearer.cxx \
	msg/IEs/IeCause.cxx \
	msg/IEs/IeCallState.cxx \
	msg/IEs/IeChanId.cxx \
	msg/IEs/IeNotify.cxx \
	msg/IEs/IeProgress.cxx \
	msg/IEs/IeSignal.cxx \
	msg/IEs/IeContent.cxx \
	msg/IEs/IeFacility.cxx \
	msg/IEs/IeCallId.cxx \
	msg/IEs/IeDisplay.cxx \
	msg/IEs/IeKeypad.cxx \
	msg/IEs/IeUserUser.cxx \
	msg/IEs/IeCallSub.cxx \
	msg/IEs/IeCalledPN.cxx \
	msg/IEs/IePN.cxx \
	msg/IEs/IeCallingPN.cxx \
	msg/IEs/IeRedirDN.cxx \
	msg/IEs/IeRedirNR.cxx \
	msg/IsdnMsg.cxx \
	msg/msg/MsgAlerting.cxx \
	msg/msg/MsgCCNew.cxx \
	msg/msg/MsgCCRelease.cxx \
	msg/msg/MsgCCTimeout.cxx \
	msg/msg/MsgConnect.cxx \
	msg/msg/MsgConnectAck.cxx \
	msg/msg/MsgDisConnect.cxx \
	msg/msg/MsgFacility.cxx \
	msg/msg/MsgHold.cxx \
	msg/msg/MsgInfo.cxx \
	msg/msg/MsgNotify.cxx \
	msg/msg/MsgProceeding.cxx \
	msg/msg/MsgRelease.cxx \
	msg/msg/MsgReleaseComplete.cxx \
	msg/msg/MsgResume.cxx \
	msg/msg/MsgSuspend.cxx \
	msg/msg/MsgSetup.cxx \
	msg/MsgCodec.cxx \
	msg/MsgDecoder.cxx \
	msg/MsgDecoderTE.cxx \
	msg/MsgDecoderNT.cxx \
	msg/MsgEncoder.cxx \
	msg/MsgEncoderTE.cxx \
	msg/MsgEncoderNT.cxx \
	msg/IsdnMsgFactory.cxx \
	msg/IEs/IeCharge.cxx 

#	msg/IEs/IeCallState.cxx \
#	msg/IEs/IeCentrex.cxx \

ISDN_2 += \
	isdn/IeDecoder.cxx \
	isdn/IsdnIeFactory.cxx \

# 以下的消息在INS中没有定义
#	isdn/msg/MsgRetrieve.cxx \
#	isdn/msg/MsgSetupAck.cxx \


# 以下IE在INS网络中不提供
#	isdn/IEs/IeConnectPN.cxx \

# 以下的IE是定义而不提供的
#	isdn/IEs/IeComplete.cxx \
#	isdn/IEs/IeDate.cxx \

		
STATE_SRC += \
	state/IsdnStateBuilder.cxx 

-include driver/Makefile.src 	

SRC += \
	$(DRIVER_SRC) \
	$(DEVICE_SRC) \
	$(EVENT_SRC) \
	$(OP_SRC) \
	$(STATE_SRC) \
	$(MSG_SRC) \
	isdn.cxx 

MY_INC_DIR = \
	-I$(ISDN_DIR)/ \
	-I$(ISDN_DIR)/device \
	-I$(ISDN_DIR)/event \
	-I$(ISDN_DIR)/state \
	-I$(ISDN_DIR)/op \
	-I$(ISDN_DIR)/msg/IEs \
	-I$(ISDN_DIR)/msg/msg \
	-I$(ISDN_DIR)/msg \
	-I$(ISDN_DIR)/driver/include 


C_FLAGS	+= -I./include  $(DRIVER_CFLAGS) \
	$(UTILS_INC_DIR) 


INCLUDE_DIR=  \
	-I$(TOP_DIR)/include -I$(TOP_DIR)/include/build \
	$(DRIVER_INC_DIR) \
	$(UTILS_INC_DIR) \
	$(PBX_INC_DIR) \
	$(MY_INC_DIR) 
	
KINCLUDES = /works/kernel/linux-2.4.x/drivers/assist/isdn 

C_CFLAGS+= -D_REENTRANT -DASSIST_USE_POSIX_THREADS -DASSIST_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM -DNO_POLICY \
		 $(COMPILE_OPTIONS) -I$(KINCLUDES) -I$(DRIVER_CFLAGS) 
		 
C_FLAGS += $(DRIVER_CFLAGS) 

TARGET_LIB:=ASVPBXPlugInIsdn
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -L$(TOP_DIR)/bin/lib/arm -L$(BIN_DIR) -L$(LIBS_DIR)/libs/bin/$(ARCH) 

# LIBS += -lisdn -lvpbxutil 
# LIBS += -lpthread -lm -lstdc++  

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	tar cvzf  p.tar.gz $@
	rm $@
	# $(MOVE) $@ $(BIN_DIR)
	# tar cvzf p.tar.gz $(BIN_DIR)/$@tar cvzf p.tar.gz $(BIN_DIR)/$@
