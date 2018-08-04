# $Id: Makefile,v 1.1.1.1 2006/11/29 09:28:03 lizhijie Exp $

# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/29 09:28:03  lizhijie
# AS600 VoipLib
#
# Revision 1.1.1.1  2006/07/12 14:44:46  lizhijie
# rebuild
#
# Revision 1.1.1.1  2005/06/09 05:37:05  tangr
# version: 01.0.00-00
#  -new voips libs and includes
#  -extract include files into include/
#  -update Makefiles in subdirs
#
# Revision 1.1.1.1  2005/03/14 07:15:16  lizhijie
# common software packages for VOIPS
#
# Revision 1.1.1.1  2005/02/01 03:55:11  lizhijie
# VOIP protocols soruces code from VOCAL
#
# Revision 1.1.1.1  2004/12/23 05:42:32  vivian
# sip
#
# Revision 1.1.1.1  2004/11/06 08:58:30  lizhijie
# phone for IXP422
#
# Revision 1.1.1.1  2004/11/05 09:05:04  vivian
# sip
#
# Revision 1.1.1.1  2004/09/21 01:20:40  yanpeng
# ixp_phone
#
# Revision 1.1  2000/06/20 02:57:35  chok
# Add test program to generate boring messages
#

BUILD = ../../../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

PACKAGES += PTHREAD UTIL

TESTPROGRAMS = \
	udpMsgTest.cxx \

SRC = \

include $(BUILD)/Makefile.post
