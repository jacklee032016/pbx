#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
# AS800 PstnTest
#
# Revision 1.2  2006/01/17 11:00:51  lizhijie
# dd
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
# Revision 1.2  2004/12/11 05:44:10  lizhijie
# some comile warning
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

DELETE_LIST:= caller callee caller_id fxo_ring fxo_rcv paral

LDFLAGS+=-L$(BIN_DIR) -lpstn_test -lpthread -lm 

LOCAL_LAST=program

$(LOCAL_LAST):$(OBJS) caller callee caller_id fxo_ring fxo_rcv paral 


############ Program 1
caller:fxo_caller.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

caller_id:fxo_caller_id.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

callee:fxo_callee.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) 
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

fxo_ring:fxo_ring.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

fxo_rcv:fxo_rcv.c 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	$(STRIP)  $@
	$(MOVE) $@ $(BIN_DIR)

paral:parallel_fxo_fxs.c
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

