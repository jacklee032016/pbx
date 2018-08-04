#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
#

CONSOLE_DIR:=$(shell pwd)

TOP_DIR:=$(shell pwd)/../..

include $(TOP_DIR)/Makefile.pre

CODE_SUBDIRS = 	\
	device event \
	state state/out state/in \
	op op/out op/in  
	

# name of all files to be compiled (don't include header files)
################################################
####   source file 
################################################
DEVICE_SRC += \
	device/ConsoleDeviceMgr.cxx \
	device/ConsoleCtrlDevice.cxx \
	device/ConsoleAudioChannel.cxx \
	device/ConsoleEndPoint.cxx \
	device/ConsoleTask.cxx 
	
OP_SRC += \
	op/ConsoleOperator.cxx \
	op/ConsoleOpBuilder.cxx \
	op/ConsoleOpDisconnect.cxx \
	op/in/ConsoleOpInCallAnswerTimer.cxx \
	op/in/ConsoleOpInCallTimeout.cxx \
	op/in/ConsoleOpInCallReDial.cxx \
	op/in/ConsoleOpInCallSetup.cxx \
	op/out/ConsoleOpOutCallAlerting.cxx \
	op/out/ConsoleOpOutCallConnect.cxx \
	op/out/ConsoleOpOutCallConnectAck.cxx \
	op/out/ConsoleOpOutCallProceeding.cxx \
	op/out/ConsoleOpOutCallRelease.cxx \
	op/out/ConsoleOpOutCallTimeout.cxx \
	op/ConsoleOpStopDialTone.cxx \
	op/ConsoleOpSetupInCall.cxx 

	
STATE_SRC += \
	state/ConsoleStateBuilder.cxx 

SRC += \
	$(DSP_SRC) \
	$(DEVICE_SRC) \
	$(OP_SRC) \
	$(STATE_SRC) \
	console.cxx 

MY_INC_DIR = \
	-I$(CONSOLE_DIR)/ \
	-I$(CONSOLE_DIR)/device \
	-I$(CONSOLE_DIR)/event \
	-I$(CONSOLE_DIR)/state \
	-I$(CONSOLE_DIR)/state/out \
	-I$(CONSOLE_DIR)/state/in \
	-I$(CONSOLE_DIR)/op/out \
	-I$(CONSOLE_DIR)/op/in \
	-I$(CONSOLE_DIR)/op 
	

C_FLAGS	+= 	
	
INCLUDE_DIR=  \
	-I$(TOP_DIR)/include \
	$(UTILS_INC_DIR) \
	$(PBX_INC_DIR) \
	$(MY_INC_DIR) 
	

C_CFLAGS+= -D_REENTRANT -DASSIST_USE_POSIX_THREADS -DASSIST_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM -DNO_POLICY \
		 $(COMPILE_OPTIONS)  
		 
C_FLAGS +=  

TARGET_LIB:=ASVPBXPlugInConsole
TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -L$(TOP_DIR)/bin/lib/arm -L$(BIN_DIR) -L$(LIBS_DIR)/libs/bin/$(ARCH) 

LIBS += -lpstn 
#LIBS += -lvpbxutil -lpstn 
#LIBS += -lpthread -lm -lstdc++  

# -Wl,-verbose -Wl,-static-static  
$(LOCAL_ALL):$(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
	$(STRIP)  $@
	tar cvzf  p.tar.gz $@
	rm $@
	# $(MOVE) $@ $(BIN_DIR)
	# tar cvzf p.tar.gz $(BIN_DIR)/$@tar cvzf p.tar.gz $(BIN_DIR)/$@
