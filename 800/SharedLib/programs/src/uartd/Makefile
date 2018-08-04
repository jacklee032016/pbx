#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
# AS800 SharedLib
#
# Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
# no message
#
# Revision 1.2  2005/06/20 09:36:15  lijie
# 串口bug修正,消息队列封装到共享库
#
# Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
# add shared library and programs depend on it
#
# $Revision: 1.1.1.1 $
# $Id: Makefile,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
#


include $(TOP_DIR)/Makefile.pre

LDFLAGS += -lpthread 

SRC= 		as_uartd_threads.c \
		as_uartd_main.c 

# all: drivers

#clean:
#	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) clean ) ; done
#	- find . -name '*.d' -print -exec /bin/rm -f {} \;

LOCAL_LAST := uartd 

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

	
