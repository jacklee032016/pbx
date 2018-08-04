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
# Revision 1.3  2004/12/14 12:48:50  lizhijie
# support building header files in the architecture platform
#
# Revision 1.2  2004/12/11 05:43:46  lizhijie
# some comile warning
#
# Revision 1.1  2004/11/25 07:36:12  lizhijie
# create the sub-directories for different test
#
# $Revision: 1.1.1.1 $
#

include $(ROOT_DIR)/Makefile.pre

KERNEL=no


CODE_SUBDIRS = 

DELETE_LIST:= test_rw2 test_rw
 

LDFLAGS+=-L$(BIN_DIR) -lpstn_test -lpthread -lm 

LOCAL_LAST=program

$(LOCAL_LAST):$(OBJS) $(DELETE_LIST)


############ Program 1
test_rw2:test_rw_2.c  
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

test_rw:test_rw.c  
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

