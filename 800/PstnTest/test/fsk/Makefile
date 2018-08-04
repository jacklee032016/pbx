#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
# AS800 PstnTest
#
# Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
# PSTN test library
#
# Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
# new drivers for all devices
#
# Revision 1.4  2004/12/20 03:18:15  lizhijie
# Add DSP library for FSK detect/generate, DTMF detect and other test code
#
# Revision 1.3  2004/12/14 12:48:50  lizhijie
# support building header files in the architecture platform
#
# Revision 1.2  2004/12/11 05:43:58  lizhijie
# some comile warning
#
# Revision 1.1  2004/12/09 07:47:42  lizhijie
# add FSK gen test program into CVS
#
# Revision 1.1  2004/11/25 07:36:12  lizhijie
# create the sub-directories for different test
#
# Revision 1.2  2004/11/22 01:54:04  lizhijie
# add some user module into CVS
#
# Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
# driver for assist telephone cards Tiger320-Si3210/3050
#
# $Revision: 1.1.1.1 $
#

include $(ROOT_DIR)/Makefile.pre

KERNEL=no

CODE_SUBDIRS = 

DELETE_LIST:= dsp_fsk_gen dsp_dtmf_gen dsp_tone_gen

LDFLAGS+=-L$(BIN_DIR) -lpstn_test -lpthread -lm 

LOCAL_LAST=program

$(LOCAL_LAST):$(OBJS) $(DELETE_LIST)


############ Program 

dsp_fsk_gen:dsp_fsk_gen.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

dsp_dtmf_gen:dsp_dtmf_gen.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

dsp_tone_gen:dsp_tone_gen.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

LOCAL_CLEAN:=clear

$(LOCAL_CLEAN):
	rm -f *.o *.~ *.*~
	for n in $(DELETE_LIST);do \
		if test -f "$$n"; then \
			$(RM) "$$n";\
	  else :; fi; \
	done

include $(ROOT_DIR)/Makefile.post

