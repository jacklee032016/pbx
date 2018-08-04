#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
#

PSTN_DIR:=$(shell pwd)

TOP_DIR:=$(shell pwd)/../..

include $(TOP_DIR)/Makefile.pre

CODE_SUBDIRS = 	\
	device event state op \
	pstn  

# name of all files to be compiled (don't include header files)

################################################
####   source file 
################################################
DEVICE_SRC = \
	device/PstnDeviceMgr.cxx \
	device/PstnCtrlDevice.cxx \
	device/PstnEndPoint.cxx \
	device/PstnAudioChannel.cxx
	
EVENT_SRC = \
	event/PstnEvent.cxx 
	
OP_SRC += \
	op/PstnOperator.cxx  \
	op/PstnOpBuilder.cxx \
	op/PstnOpDialError.cxx \
	op/PstnOpOnHook.cxx \
	op/PstnOpRing.cxx \
	op/PstnOpSetup.cxx \
	op/PstnOpStartCall.cxx \
	op/PstnOpStartDialTone.cxx \
	op/PstnOpStopDialTone.cxx \
	op/PstnOpStartErrorTone.cxx \
	op/PstnOpStartRingbackTone.cxx \
	op/PstnOpFarEndAnswered.cxx \
	op/PstnOpFarEndError.cxx \
	op/PstnOpTimeout.cxx \
	op/PstnOpTerminateTrying.cxx \
	op/PstnOpEndCall.cxx \
	op/PstnOpTerminateCall.cxx \
	op/PstnOpStopRinging.cxx \
	op/PstnOpAnswerCall.cxx \
	op/PstnOpRingCancel.cxx \
	op/PstnOpDeviceStatus.cxx \
	op/PstnOpQueryStatus.cxx \
	op/PstnOpPrimAnswer.cxx \
	op/PstnOpEndCar.cxx \
	op/PstnOpFwdDigitInCall.cxx \
	op/PstnOpSetupInCall.cxx 

STATE_SRC = \
	state/PstnStateBuilder.cxx 

DRIVER_SRC = \
	pstn/PstnTaskEngine.cxx \
	pstn/assist_init.cxx  

#	pstn/PstnThread.cxx \
	
SRC += \
	$(DRIVER_SRC) \
	$(DEVICE_SRC) \
	$(EVENT_SRC) \
	$(OP_SRC) \
	$(STATE_SRC) \
	pstn.cxx 

MY_INC_DIR = \
	-I$(PSTN_DIR)/ \
	-I$(PSTN_DIR)/device \
	-I$(PSTN_DIR)/event \
	-I$(PSTN_DIR)/state \
	-I$(PSTN_DIR)/op \
	-I$(PSTN_DIR)/pstn 

C_FLAGS += -I./pstn
C_FlAGS += -I./include

INCLUDE_DIR=  \
	-I$(TOP_DIR)/include/build \
	-I$(TOP_DIR)/include \
	$(DRIVER_INC_DIR) \
	$(UTILS_INC_DIR) \
	$(PBX_INC_DIR) \
	$(MY_INC_DIR)
	

C_CFLAGS+= -D_REENTRANT -DASSIST_USE_POSIX_THREADS -DASSIST_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM -DNO_POLICY \
		 $(COMPILE_OPTIONS) $(CFLAGS) 
		 

TARGET_LIB:=ASVPBXPlugInPstn
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -export-dynamic -L$(TOP_DIR)/bin/lib/arm -L$(BIN_DIR) -L$(LIBS_DIR)/libs/bin/$(ARCH)

LIBS +=  -lpstn 
# LIBS += -lvpbxutil -lpstn 
# LIBS += -lpthread -lm -lstdc++  
#libpstn.so
LIBS += -lpstn

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	tar cvzf  p.tar.gz $@
	rm $@
	# $(MOVE) $@ $(BIN_DIR)
	# tar cvzf p.tar.gz $(BIN_DIR)/$@tar cvzf p.tar.gz $(BIN_DIR)/$@
