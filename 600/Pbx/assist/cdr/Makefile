#Makefile for Cdr

TOP_DIR:=$(shell pwd)

export TOP_DIR


include $(TOP_DIR)/Makefile.pre

SUBDIRS = # test

SPROGS=	cdr TestCdr

CDR_OBJS = $(OBJ_DIR)/Cdr.o \
		$(OBJ_DIR)/CdrDataServer.o \
		$(OBJ_DIR)/helper.o

TEST_OBJS = $(OBJ_DIR)/TestCdr.o	

LOCAL_ALL=$(SPROGS)

SRC = 	Cdr.cxx \
        CdrDataServer.cxx \
        helper.cxx \
        TestCdr.cxx
	


all: 
	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) all ) ; done	
	
TestCdr:$(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^  $(L_LIBS)
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)


cdr:$(CDR_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^  -lpthread
	$(STRIP) $@
	$(MOVE) $@ $(BIN_DIR)
	$(MKDIR) ../$(OBJ_DIR)/cdr
	$(COPY) -f $(BIN_DIR)/* ../$(OBJ_DIR)/cdr/

	
install:
	mkdir -p $(TOP_DIR)/releases/etc/sip/cdr/records
	mkdir -p $(TOP_DIR)/releases/usr/sbin
	rm -r -f assist_cdr_*.tar
	rm -r -f ../$(OBJ_DIR)/cdr/assist_cdr_*.tar
	cp $(BIN_DIR)/* $(TOP_DIR)/releases/usr/sbin
	tar cvf $(ASSIST_RELEASES_NAME) -C releases etc usr
	mv $(ASSIST_RELEASES_NAME) ../$(OBJ_DIR)/cdr/

releasedir:
	mkdir -p release/bin.$(TARGET_NAME)
	mkdir -p release/lib.$(TARGET_NAME)

include $(TOP_DIR)/Makefile.post


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


# clean:
#	- for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) clean ) ; done
#	- find . -name '*.d' -print -exec /bin/rm -f {} \;
