#
# $Id: Makefile,v 1.2 2007/03/02 04:19:27 lizhijie Exp $


include $(ROOT_DIR)/Makefile.pre

PACKAGES += SDP2 UTIL PTHREAD LIBXML

CODE_SUBDIRS = 

# name of all files to be compiled (don't include header files)
SRC = \
	Builder.cxx \
	CallContainer.cxx \
	CallInfo.cxx \
	DeviceEvent.cxx \
	SipProxyEvent.cxx \
	Feature.cxx \
	Operator.cxx \
	SipEvent.cxx \
	SipThread.cxx \
	State.cxx \
	TimerEvent.cxx \
	WorkerThread.cxx \
	HeartLessProxy.cxx \
	StateMachnData.cxx \
	BaseAuthentication.cxx \
	CommonFunctions.cxx 
	
#	BaseCommandLine.cxx \
#	StatelessWorkerThread.cxx 

ifeq ($(USE_HEARTBEAT),1)
SRC += 	BasicProxy.cxx \
	StatelessBasicProxy.cxx 
# TESTPROGRAMS +=  BaseProxyTestMain.cxx
endif


INCLUDE_DIR= \
	$(UTILS_INC_DIR) \
	$(SDP_INC_DIR) \
	$(RTP_INC_DIR) \
	$(SIPSTACK_INC_DIR) \
	$(BASE_INC_DIR) \
	$(UA_INC_DIR) 


C_CFLAGS+=  -fPIC -D_REENTRANT -DUSE_PTHREADS -DVOCAL_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM
		 
ifeq ($(CODE),uart)
	C_CFLAGS += -DDEBUG_UART=1
else 
	C_CFLAGS += -DDEBUG_UART=0
endif


TARGET_LIB = libvoip_proxyBase

include $(ROOT_DIR)/Makefile.post
