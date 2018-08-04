# $Id: Makefile,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

PACKAGES += SDP2 UTIL PTHREAD LIBXML

CODE_SUBDIRS = pbx

# name of all files to be compiled (don't include header files)
SRC += \
	OpAck.cxx \
	OpAckReInvite.cxx \
	OpActiveCallEnded.cxx \
	OpAddDigit.cxx \
	OpAnswerCall.cxx \
	OpBeingXferred.cxx \
	OpBlindXferHoldOk.cxx \
	OpBlindXferred.cxx \
	OpBusyAck.cxx \
	OpBusyHere.cxx \
	OpCancelTimer.cxx \
	OpConfComplete.cxx \
	OpConfError.cxx \
	OpConfTargetOk.cxx \
	OpConferencing.cxx \
	OpConsultXferred.cxx \
	OpConvertCw.cxx \
	OpCwRing.cxx \
	OpDialError.cxx \
	OpDigitsResp.cxx \
	OpEndCall.cxx \
	OpFarEndAnswered.cxx \
	OpFarEndBusy.cxx \
	OpFarEndError.cxx \
	OpFwdDigit.cxx \
	OpHangupError.cxx \
	OpHeldCallEndedDialing.cxx \
	OpHeldCallEndedIn2Calls.cxx \
	OpHeldCallEndedTrying.cxx \
	OpHoldTargetErr.cxx \
	OpHoldTargetOk.cxx \
	OpInCallBusy.cxx \
	OpInCallFarEndAnswered.cxx \
	OpInCallFastBusy.cxx \
	OpInCallInvite.cxx \
	OpInCallStopTrying.cxx \
	OpInviteUrl.cxx \
	OpOnHook.cxx \
	OpReDirect.cxx \
	OpReInvite.cxx \
	OpReInviteAuthenticated.cxx \
	OpReRing.cxx \
	OpRecvdXfer.cxx \
	OpRevert.cxx \
	OpRing.cxx \
	OpRingCancel.cxx \
	OpRingingInvite.cxx \
	OpSecondCall.cxx \
	OpStartAudioDuplex.cxx \
	OpStartBlindXferring.cxx \
	OpStartBusyTone.cxx \
	OpStartCall.cxx \
	OpStartCallWaiting.cxx \
	OpStartConferencing.cxx \
	OpStartConsultXferring.cxx \
	OpStartDialTone.cxx \
	OpStartErrorTone.cxx \
	OpStartFastBusyTone.cxx \
	OpStartRingbackTone.cxx \
	OpStartRinging.cxx \
	OpStartTimer.cxx \
	OpStopAudio.cxx \
	OpStopBusyTone.cxx \
	OpStopDialTone.cxx \
	OpStopErrorTone.cxx \
	OpStopFastBusyTone.cxx \
	OpStopRingbackTone.cxx \
	OpStopRinging.cxx \
	OpSuspendAudio.cxx \
	OpSwapCalls.cxx \
	OpSwapComplete.cxx \
	OpTerminateCall.cxx \
	OpTerminateTrying.cxx \
	OpTimeout.cxx \
	OpXferComplete.cxx \
	OpXferDenied.cxx \
	OpXferEnded.cxx \
	OpXferHold.cxx \
	OpXferTargetHoldOk.cxx \
	OpXferTargetInviteOk.cxx \
	OpXfereeComplete.cxx \
	OpQueryStatus.cxx \
	OpDeviceIdle.cxx \
	OpDeviceBusy.cxx \
	OpTimeOver.cxx \
	OpTimeFinish.cxx \
	OpDropTalk.cxx \
	OpAuthentication.cxx \
	OpAuthorization.cxx \
	pbx/OpTimePbxEnd.cxx \
	pbx/OpBranchBusy.cxx \
	pbx/OpBranchIdle.cxx \
	pbx/OpQueryPbxStatus.cxx \
	pbx/OpCwPbxRing.cxx \
	pbx/OpStopPbxRinging.cxx \
	pbx/OpAddPbxDigit.cxx \
	pbx/OpPbxDialError.cxx \
	pbx/OpPbxDialOK.cxx \
	pbx/OpPbxHookUp.cxx \
	pbx/OpStopPbxDialing.cxx 

#	pbx/OpPbxEnd.cxx
#	OpAutoAnswer.cxx \
#	OpAutoCall.cxx \
#	OpAutoCalleeInCallTO.cxx \
#	OpAutoCallerInCallTO.cxx \
#	OpAutoEndRecCall.cxx \
#	OpAutoFarEndAnswered.cxx \
#	OpAutoProcessOk.cxx \
#	OpAutoRS.cxx \
#	OpAutoRSAck.cxx	\
#	OpAutoTerminateCall.cxx \
#	OpAutoTryingTO.cxx \


INCLUDE_DIR=  \
	-I$(ROOT_DIR)/include \
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


TARGET_LIB = libvoip_uaop

include $(ROOT_DIR)/Makefile.post

LDFLAGS += -L$(VOIPS_DIR)/bin/$(ARCH) -L$(BIN_DIR) 
OP_CDR_OBJS = $(OBJ_DIR)/OpStartAudioDuplex.o $(OBJ_DIR)/OpEndCall.o \
	$(OBJ_DIR)/OpRing.o $(OBJ_DIR)/OpRingCancel.o $(OBJ_DIR)/OpTerminateCall.o
cleancdr:
	rm -r -f $(OP_CDR_OBJS)
