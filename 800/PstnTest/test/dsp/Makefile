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
# Revision 1.1  2004/12/20 03:22:42  lizhijie
# add test for FSK generate and detect for test into CVS
#
# $Revision: 1.1.1.1 $
#

include $(ROOT_DIR)/Makefile.pre

KERNEL=no

CODE_SUBDIRS = 

DELETE_LIST:= dsp_gain_test dsp_fsk_test_detect

#dsp_fsk_test_file busy_tone_gen  


LDFLAGS+=-L$(BIN_DIR) -lpstn_test -lpthread -lm 

LOCAL_LAST=program

$(LOCAL_LAST):$(OBJS) $(DELETE_LIST)


############ Program 

busy_tone_gen:busy_tone_gen.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

dsp_gain_test:dsp_gain_test.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

dsp_fsk_test_file:dsp_fsk_test_file.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

dsp_fsk_test_detect:dsp_fsk_test_detect.c 
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
