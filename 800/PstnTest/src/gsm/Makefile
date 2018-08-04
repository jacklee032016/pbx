# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
# AS800 PstnTest
#
# Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
# PSTN test library
#
# Revision 1.1.1.1  2004/11/08 02:02:14  lizhijie
# GSM library
#
# $Revision: 1.1.1.1 $

# Makefile for gsmlib, created by lizhijie 2004.11.06


LIB_OBJS :=\
	add.o code.o debug.o decode.o long_term.o \
	lpc.o preprocess.o rpe.o gsm_destroy.o gsm_decode.o \
	gsm_encode.o gsm_explode.o gsm_implode.o gsm_create.o \
	gsm_print.o gsm_option.o short_term.o table.o

include $(TOPDIR)/Makefile.include

SHARE_LIB:=libgsm.so
STATIC_LIB:=libgsm.a

EXECS:= $(STATIC_LIB) $(SHARE_LIB) 

$(STATIC_LIB):$(LIB_OBJS)
	$(AR) cru $@ $^
	$(RANLIB) $@
	$(COPY) $(STATIC_LIB) $(LOCAL_RELEASE_DIR)/lib

$(SHARE_LIB):$(LIB_OBJS)
	$(CC) -Wl,-shared $(LDFLAGS) $^ -o $@
	$(COPY) $(SHARE_LIB) $(LOCAL_RELEASE_DIR)/lib
	
EXE:$(EXECS) #$(objs)

clean:
	$(RM) *.o *.*~ core $(EXECS)
