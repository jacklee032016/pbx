# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/29 09:28:02  lizhijie
# AS600 VoipLib
#
# Revision 1.1.1.1  2006/07/12 14:44:45  lizhijie
# rebuild
#
# $Id: Makefile,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $

include $(ROOT_DIR)/Makefile.pre

# C_CFLAGS	+= $(RTP_INC_DIR) 
# C_FLAGS		+= $(RTP_INC_DIR) 

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


#CAPIBUILDER := $(BUILD)/../tools/cApiBuilder

#UTILS_DIR=$(TOP_DIR)/../$(GENERIC_INC_DIR)/util
#RTP_DIR=$(TOP_DIR)/../$(GENERIC_INC_DIR)/rtp

TARGET_LIB = libvoip_rtp

include $(ROOT_DIR)/Makefile.post
