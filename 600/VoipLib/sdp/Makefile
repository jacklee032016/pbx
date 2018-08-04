# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/29 09:28:02  lizhijie
# AS600 VoipLib
#
# Revision 1.1.1.1  2006/07/12 14:44:46  lizhijie
# rebuild
#
# $Id: Makefile,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

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


# CAPIBUILDER := $(BUILD)/../tools/cApiBuilder

# UTILS_DIR=$(TOP_DIR)/../$(GENERIC_INC_DIR)/util
#/util
# SIP_DIR=$(TOP_DIR)/sip
# SDP_DIR=$(TOP_DIR)/../$(GENERIC_INC_DIR)/sdp
# RTP_DIR=$(TOP_DIR)/../$(GENERIC_INC_DIR)/rtp
# MEDIA_DIR=$(TOP_DIR)/libmedia


TARGET_LIB = libvoip_sdp

include $(ROOT_DIR)/Makefile.post
