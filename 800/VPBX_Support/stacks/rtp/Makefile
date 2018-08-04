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

PACKAGES += PTHREAD 


CODE_SUBDIRS = codec

# name of all files to be compiled (don't include header files)
SRC = \
	RtpPacket.cxx \
	RtpTransmitter.cxx \
	RtpReceiver.cxx \
	RtcpPacket.cxx \
	RtcpTransmitter.cxx \
	RtcpReceiver.cxx \
	RtpEvent.cxx \
	RtpEventReceiver.cxx \
	RtpSession.cxx \
	NtpTime.cxx \
	rtpTools.cxx \
        rtpCodec.cxx \
        codec/g711.c \
        codec/g721.c \
        codec/g723_24.c \
        codec/g723_40.c \
        codec/g72x.c


CAPIBUILDER := $(BUILD)/../tools/cApiBuilder


UTILS_DIR=$(TOP_DIR)/../../include/util
RTP_DIR=$(TOP_DIR)/../../$(GENERIC_INC_DIR)/stacks/rtp

TARGET_LIB = $(BIN_DIR)/librtp

TOP_DIR:=$(shell pwd)
include $(TOP_DIR)/Makefile.post
