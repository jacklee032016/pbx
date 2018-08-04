
include $(TOP_DIR)/Makefile.pre

#LDFLAGS += -lpthread 

SRC= 	as_lcd_test.c

# all: drivers

#clean:
#	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) clean ) ; done
#	- find . -name '*.d' -print -exec /bin/rm -f {} \;

LOCAL_LAST := lcdtest 

include $(TOP_DIR)/Makefile.post

#LDFLAGS += -()

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

	
