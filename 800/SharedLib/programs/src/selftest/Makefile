#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
# AS800 SharedLib
#
# Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
# no message
#
# Revision 1.5  2005/09/16 09:30:15  wangwei
# no message
#
# Revision 1.4  2005/09/05 07:15:48  wangwei
# no message
#
# Revision 1.3  2005/06/24 05:25:59  wangwei
# ‘ˆº”±‡“Î as_test_dev_cf_card.c
#
# Revision 1.2  2005/06/03 04:01:19  lizhijie
# add defination for NPE test
#
# Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
# add shared library and programs depend on it
#
# $Revision: 1.1.1.1 $
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
#


include $(TOP_DIR)/Makefile.pre

SRC += 	as_test_main.c 
SRC += 	as_test_result.c 
SRC += 	as_test_net_link.c 
SRC += 	as_test_serial_no.c 
SRC += 	as_test_drivers.c 

ifeq ($(ZARLINK_SUP),yes)
	CFLAGS += -DZARLINK_SUPPORT=1 
endif

ifeq ($(PCM_SUP),yes)
	SRC += 	as_test_pcm_channels.c 
	CFLAGS += -DPCM_SUPPORT=1 
endif

ifeq ($(CF_SUP),yes)
	SRC += 	as_test_dev_cf_card.c
	CFLAGS += -DCF_SUPPORT=1 
endif

ifeq ($(LED_SUP),no)
else
	SRC += 	as_test_dev_led.c
	CFLAGS += -DLED_SUPPORT=1 
endif

ifeq ($(ETH2_SUP),no)
else
	CFLAGS += -DETH2_SUPPORT=1 
endif



# all: drivers

#clean:
#	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) clean ) ; done
#	- find . -name '*.d' -print -exec /bin/rm -f {} \;

LOCAL_LAST := selftest 

include $(TOP_DIR)/Makefile.post


$(LOCAL_LAST):$(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR) 

#releases/usr/sbin  -lcap -lcrypt 
	
cleanall:
	find . -name 'obj.*' -print -exec /bin/rm -rf {} \;
	find . -name 'bin.*' -print -exec /bin/rm -rf {} \;
	find . -name '*.d' -print -exec /bin/rm -f {} \;
	find . -name '.link_host' -print -exec /bin/rm -f {} \;
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) cleanall ) ; done
	rm -rf release/bin.*/*
	rm -rf release/lib.*/*
	rm -rf release/include/*

veryclean: clean
	- find . -name 'obj.*' -print -exec /bin/rm -rf {} \;
	- find . -name 'bin.*' -print -exec /bin/rm -rf {} \;
	- find . -name '*.o' -print -exec /bin/rm -f {} \;
	- find . -name '*.a' -print -exec /bin/rm -f {} \;
	- find . -name 'core' -print -exec /bin/rm -f {} \;
	- find . -name '.link_host' -print -exec /bin/rm -f {} \;

install:
	- find releases -name \CVS -exec rm -r -f {} \;
# add var direct by fengshikui 2005-1-10 15:51
	tar cvf $(ASSIST_RELEASES_NAME) -C releases etc usr var  
	mv $(ASSIST_RELEASES_NAME)  $(ASSIST_IXP_RELEASES_HOME)

	
