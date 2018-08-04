# $Id: Makefile,v 1.1.1.1 2006/11/29 09:28:03 lizhijie Exp $

BUILD = ../../build

include $(BUILD)/Makefile.pre

DEFINES += USE_PTHREADS

PACKAGES += UTIL LIBCEXT PTHREAD LIBXML OPENSSL


# names of programs (e.g. test programs) to be compiled

VERIFY_TESTCASES = \
	VerifyTest.cxx \
	VerifyTestShort.cxx\
	UdpStackVerify.cxx\
	NetworkAddressVerify.cxx\
	Md5Test.cxx\
	VerifyExample01.cxx \
	VerifyExample02.cxx \
	VerifyExample03.cxx \
	SptrTest08.cxx\
	CircularBufferTest.cxx\
	DataVerify.cxx

include $(BUILD)/Makefile.post
