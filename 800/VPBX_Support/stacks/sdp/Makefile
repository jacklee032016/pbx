# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
# AS800 VPBX_Support
#
# Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
# build a independent directory for VPBX support libraris
#
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $

TOP_DIR:=$(shell pwd)

GENERIC_INC_DIR = include
BUILD = $(TOP_DIR)/../../$(GENERIC_INC_DIR)/build

include $(TOP_DIR)/Makefile.pre

PACKAGES += UTIL LIBXML PTHREAD 

CODE_SUBDIRS = sdp2

# name of all files to be compiled (don't include header files)
SRC = \
	SdpSession.cxx \
	SdpMedia.cxx \
	SdpAttributes.cxx \
	SdpTime.cxx \
	SdpBandwidth.cxx \
	SdpConnection.cxx \
	SdpEncryptkey.cxx \
	sdp2/Sdp2Session.cxx \
	sdp2/Sdp2Media.cxx \
	sdp2/Sdp2Attributes.cxx \
	sdp2/Sdp2Time.cxx \
	sdp2/Sdp2Bandwidth.cxx \
	sdp2/Sdp2Connection.cxx \
	sdp2/Sdp2Encryptkey.cxx 


CAPIBUILDER := $(BUILD)/../tools/cApiBuilder

UTILS_DIR=$(TOP_DIR)/../../$(GENERIC_INC_DIR)/util

#/util
SIP_DIR=$(TOP_DIR)/sip
SDP_DIR=$(TOP_DIR)/../../$(GENERIC_INC_DIR)/stacks/sdp
RTP_DIR=$(TOP_DIR)/../../$(GENERIC_INC_DIR)/stacks/rtp
MEDIA_DIR=$(TOP_DIR)/libmedia


TARGET_LIB = $(BIN_DIR)/libsdp

include $(TOP_DIR)/Makefile.post
