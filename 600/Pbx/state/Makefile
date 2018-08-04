#
# $Id: Makefile,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

PACKAGES += SDP2 UTIL PTHREAD LIBXML

CODE_SUBDIRS = pbx

# name of all files to be compiled (don't include header files)
SRC += \
	StateBeingXferred.cxx \
	StateBlindXferring.cxx \
	StateBusy.cxx \
	StateBusyHere.cxx \
	StateCallEnded.cxx  \
	StateConferencing.cxx \
	StateConsultXferring.cxx \
	StateDialing.cxx \
	StateError.cxx \
	StateFastBusy.cxx \
	StateIdle.cxx \
	StateIn2Calls.cxx \
	StateInCall.cxx \
	StateInCallBusy.cxx \
	StateInCallDialing.cxx \
	StateInCallFastBusy.cxx \
	StateInCallTrying.cxx \
	StateOnhold.cxx \
	StateReRinging.cxx \
	StateRinging.cxx \
	StateTrying.cxx \
	pbx/StatePbxDialing.cxx \
	pbx/StatePbxRinging.cxx 

#	StatePbxInCall.cxx
#	StateAutoIdle.cxx \
#	StateAutoInCall.cxx \
#	StateAutoRS.cxx \
#	StateAutoTrying.cxx \

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
	$(UA_OP_INC_DIR) 
	

C_CFLAGS+=  -fPIC -D_REENTRANT -DUSE_PTHREADS -DVOCAL_USE_DEPRECATED \
		 -DUSE_PTHREADS -DUSE_CRYPTO_RANDOM -DNO_POLICY 
		 

TARGET_LIB = libvoip_uastate

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -L$(VOIPS_DIR)/binary -L$(BIN_DIR) 

