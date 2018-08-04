#
# $Id: Makefile,v 1.2 2007/03/01 20:04:31 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

PACKAGES += SDP2 UTIL PTHREAD LIBXML

VOCAL_DIR=$(TOP_DIR)/..

CODE_SUBDIRS = . 

# name of all files to be compiled (don't include header files)
SRC = \
	EmbeddedObj.cxx \
	AckMsg.cxx\
	BaseUrl.cxx\
	BaseUser.cxx\
	ByeMsg.cxx\
	CancelMsg.cxx\
	EmbeddedObj.cxx \
	ImUrl.cxx \
	ImpBaseUrl.cxx \
	InfoMsg.cxx\
	InviteMsg.cxx\
	MessageMsg.cxx\
	NotifyMsg.cxx\
	NullUrl.cxx\
	NullUser.cxx\
	OptionsMsg.cxx\
	ReferMsg.cxx\
	RegisterMsg.cxx\
	RetransmitContents.cxx\
	SessionTimer.cxx \
	SipAccept.cxx\
	SipAcceptEncoding.cxx\
	SipAcceptLanguage.cxx\
	SipAgent.cxx\
	SipAllow.cxx\
	SipAlso.cxx\
	SipAuthorization.cxx\
	SipBasic.cxx\
	SipCHAP.cxx\
	SipCSeq.cxx\
	SipCallId.cxx\
	SipCallLeg.cxx\
	SipCommand.cxx\
	SipContact.cxx\
	SipContentData.cxx\
	SipContentDataContainer.cxx \
	SipContentDisposition.cxx\
	SipContentEncoding.cxx\
	SipContentLanguage.cxx \
	SipContentLength.cxx\
	SipContentType.cxx\
	SipDate.cxx\
	SipDebuggingInterface.cxx \
	SipDigest.cxx\
	SipDiversion.cxx\
	SipEncryption.cxx\
	SipExpires.cxx\
	SipFragData.cxx\
	SipFrom.cxx\
	SipFtrSupportedEntry.cxx\
	SipFtrSupportedTable.cxx\
	SipHeader.cxx\
	SipHide.cxx\
	SipInReplyTo.cxx \
	SipIsup.cxx\
	SipMaxForwards.cxx\
	SipMethod.cxx \
	SipMimeVersion.cxx \
	SipMinSE.cxx \
	SipMsg.cxx\
	SipMsgQueue.cxx\
	SipOrganization.cxx\
	SipOsp.cxx\
	SipOspAuthorization.cxx \
	SipParameterList.cxx \
	SipParserMode.cxx\
	SipPriority.cxx\
	SipProxyAuthenticate.cxx\
	SipProxyAuthorization.cxx\
	SipProxyRequire.cxx\
	SipRawHeader.cxx\
	SipRawHeaderContainer.cxx\
	SipRecordRoute.cxx\
	SipReferTo.cxx\
	SipReferredBy.cxx\
	SipReplaces.cxx\
	SipRequestLine.cxx\
	SipRequire.cxx\
	SipResponseKey.cxx\
	SipRetryAfter.cxx\
	SipRoute.cxx\
	SipSdp.cxx\
	SipSentRequestDB.cxx \
	SipSentResponseDB.cxx \
	SipServer.cxx\
	SipSession.cxx \
	SipSessionExpires.cxx \
	SipSnmpDetails.cxx\
	SipStatusLine.cxx\
	SipSubject.cxx\
	SipSubsNotifyEvent.cxx\
	SipSupported.cxx \
	SipTcpConnection.cxx\
	SipTelSubscriberUser.cxx\
	SipTextData.cxx \
	SipTimestamp.cxx\
	SipTo.cxx\
	SipTransDebugger.cxx \
	SipTransHashTable.cxx \
	SipTransactionDB.cxx \
	SipTransactionGC.cxx \
	SipTransactionId.cxx \
	SipTransactionLevels.cxx \
	SipTransceiver.cxx\
	SipTransferTo.cxx\
	SipUdpConnection.cxx\
	SipUdp_impl.cxx\
	SipUnknownContentData.cxx\
	SipUnsupported.cxx\
	SipUriSupportedEntry.cxx\
	SipUriSupportedTable.cxx\
	SipUrl.cxx\
	SipUser.cxx\
	SipUserAgent.cxx\
	SipVector.cxx\
	SipVia.cxx\
	SipWarning.cxx\
	SipWwwAuthenticate.cxx\
	SnmpData.cxx\
	StatusMsg.cxx\
	SubscribeMsg.cxx\
	SystemInfo.cxx\
	TelUrl.cxx\
	TransferMsg.cxx\
	UnknownExtensionMsg.cxx \
	UnknownUrl.cxx\
	symbols.cxx \
	PresUrl.cxx 



# CAPIBUILDER := $(BUILD)/../tools/cApiBuilder

# GENERIC_INC_DIR = include
# BUILD = $(VOCAL_DIR)/$(GENERIC_INC_DIR)/build
# UTILS_DIR=$(VOCAL_DIR)/$(GENERIC_INC_DIR)/util
# SDP_DIR=$(VOCAL_DIR)/$(GENERIC_INC_DIR)/sdp
# SIPSTACK_DIR=$(VOCAL_DIR)/$(GENERIC_INC_DIR)/sipstack

TARGET_LIB = libvoip_sipstack

include $(ROOT_DIR)/Makefile.post
