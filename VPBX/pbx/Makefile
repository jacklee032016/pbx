#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
#

PBX_DIR:=$(shell pwd)

TOP_DIR:=$(shell pwd)/..

WITH_REG_EXPRESS:=
# yes

include $(TOP_DIR)/Makefile.pre

BUILDTIME := $(shell TZ=UTC date -u "+%Y_%m_%d-%H_%M")
ASSIST_RELEASES_NAME=assist_isdnpbx_$(BUILDTIME).tar 

CODE_SUBDIRS = 	\
		device device/regex \
		event event/timer \
		msg \
		op op/call \
		state \
		vpbx \
		base 

ifeq ($(test),api)
	CODE_SUBDIRS += api
endif

# name of all files to be compiled (don't include header files)

################################################
####   source file 
################################################
BASE_SRC += \
	base/PbxAgent.cxx \
	base/PbxManager.cxx \
	base/PbxManagerThread.cxx \
	base/AudioThread.cxx \
	base/DeviceCommandThread.cxx \
	base/TaskThread.cxx \
	base/ManagedObject.cxx \
	base/PbxStateObject.cxx 


DEVICE_SRC = \
	device/GatewayMgr.cxx \
	device/DeviceMgr.cxx \
	device/CtrlDevice.cxx \
	device/EndPoint.cxx \
	device/AudioChannel.cxx \
	device/DialingValidator.cxx 
	
ifeq ($(WITH_REG_EXPRESS),yes)
	DEVICE_SRC += device/regex/regex.c 
endif
	
#	device/UaConfiguration.cxx   #For temp

EVENT_SRC = \
	event/PbxEvent.cxx \
	event/CallEvent.cxx \
	event/PbxDeviceEvent.cxx \
	event/PbxHwEvent.cxx \
	event/timer/PbxTimerEvent.cxx \
	event/timer/CallTimerEvent.cxx \
	event/timer/EpTimerEvent.cxx \
	event/DigitEvent.cxx \
	event/DigitTimeoutEvent.cxx \
	event/TaskEvent.cxx 

CALLMSG_SRC += \
	msg/CommonMsg.cxx \
	msg/CallMsgBuilder.cxx 
	
OP_SRC += \
	op/PbxOperator.cxx \
	op/EpOperator.cxx \
	op/EpOpBuilder.cxx \
	op/EpOpStartTimer.cxx \
	op/EpOpCancelTimer.cxx \
	op/EpOpAddDigit.cxx \
	op/EpOpCallReleaseComplete.cxx \
	op/CallOperator.cxx \
	op/call/CallOpBuilder.cxx \
	op/call/CallOpRouting.cxx \
	op/call/CallOpProceeding.cxx \
	op/call/CallOpAlerting.cxx \
	op/call/CallOpConnect.cxx \
	op/call/CallOpRelease.cxx \
	op/call/CallOpDisconnect.cxx \
	op/call/CallOpConnectAckBeforeConnect.cxx \
	op/call/CallOpConnectAckInConnect.cxx \
	op/call/CallOpForwardSetupInCall.cxx 
		
STATE_SRC = \
	state/PbxState.cxx \
	state/EpState.cxx \
	state/CallState.cxx \
	state/CallStateBuilder.cxx 


VPBX_SRC += \
	vpbx/PbxStateMachine.cxx \
	vpbx/EpStateMachine.cxx \
	vpbx/CallId.cxx \
	vpbx/CallInfo.cxx \
	vpbx/CallContainer.cxx \
	vpbx/CallScheduler.cxx 


SRC += \
	$(BASE_SRC) \
	$(VPBX_SRC) \
	$(DEVICE_SRC) \
	$(EVENT_SRC) \
	$(CALLMSG_SRC) \
	$(OP_SRC) \
	$(STATE_SRC) \
	PbxConfiguration.cxx 

ifneq ($(test),api)
	SRC += pbx.cxx
else
	SRC += \
		api/intf.cxx \
		api/api.c
endif


################################################
####   include file 
################################################
BASE_INC_DIR = \
	-I$(PBX_DIR)/ \
	-I$(PBX_DIR)/base 

DEVICE_INC_DIR = \
	-I$(PBX_DIR)/device 	

EVENT_INC_DIR = \
	-I$(PBX_DIR)/event \
	-I$(PBX_DIR)/event/timer 

STATE_INC_DIR = \
	-I$(PBX_DIR)/state 

OP_INC_DIR = \
	-I$(PBX_DIR)/op \
	-I$(PBX_DIR)/op/call 

VPBX_INC_DIR = \
	-I$(PBX_DIR)/vpbx 

CALLMSG_INC_DIR = \
	-I$(PBX_DIR)/msg


PBX_INC_DIR= \
	$(BASE_INC_DIR) 	\
	$(DEVICE_INC_DIR) 	\
	$(EVENT_INC_DIR) 	\
	$(STATE_INC_DIR) 	\
	$(OP_INC_DIR) 		\
	$(VPBX_INC_DIR) 	\
	$(CALLMSG_INC_DIR)     

INCLUDE_DIR=  \
	-I$(TOP_DIR)/include \
	$(DRIVER_INC_DIR) \
	$(UTILS_INC_DIR) \
	$(PBX_INC_DIR)
	

C_CFLAGS+= -D_REENTRANT -DASSIST_USE_POSIX_THREADS -DASSIST_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM -DNO_POLICY \
		 $(COMPILE_OPTIONS) 

		 
ifneq ($(test),api)
	LOCAL_ALL=pbx 
else
	PBX_INC_DIR += -I$(PBX_DIR)/api
	TARGET_LIB:=libvpbx
	TARGET_LIB_SHARED:=$(TARGET_LIB).so
	TARGET_LIB_STATIC:=$(TARGET_LIB).a
endif

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -Wl,-export-dynamic -L$(TOP_DIR)/bin/lib/arm -L$(BIN_DIR) -L$(LIBS_DIR)/libs/bin/$(ARCH) \
           -L$(TOP_DIR)/shared_lib/libs/bin/$(ARCH)
LIBS += -lvpbxutil 
LIBS += -lpthread -lm -lstdc++ -ldl  

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	tar cvzf  p.tar.gz $@
	rm $@
	$(MOVE) p.tar.gz $(BIN_DIR)
	# $(MOVE) $@ $(BIN_DIR)
	# tar cvzf p.tar.gz $(BIN_DIR)/$@tar cvzf p.tar.gz $(BIN_DIR)/$@
