#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
# AS800 Kernel
#
# Revision 1.1  2006/03/13 02:44:58  lizhijie
# no message
#
# $Revision: 1.1.1.1 $
#

ARCH=arm

DELETE_LIST:= as_gen_dtmf_$(ARCH) \
	as_gen_tones_$(ARCH) \
	as_gen_hardtones \
	as_gen_hard_dtmf \
	as_dtmf_tones.h \
	as_dtmf_hard_tones.h \
	as_zone_tones.h \
	as_zone_hard_tones.h

CFLAGS += -DAS_PROSLIC_DSP=1 -D__ARM__ -I../../../include 

SRC = 	#as_tonezone_data.c 
	
all: gen_tones_$(ARCH)

# gen_dtmf_$(ARCH)

############ Program 1
as_dtmf_tones.h:gen_dtmf
#	./as_gen_dtmf
	
gen_dtmf_$(ARCH):as_gen_dtmf_tones.o 
#	$(CC) $(CFLAGS) -c as_gen_dtmf_tones.c -o as_gen_dtmf_tones.o
	$(CC) $(LDFLAGS) -o $@ $< -lm
	$(STRIP)  $@
	$(COPY) $@ $(BIN_DIR)

############ Program 2
as_zone_tones.h:as_gen_tones
#	./as_gen_tones
	
gen_tones_$(ARCH): as_gen_tones.o as_tonezone_data.o 
#	$(CC) $(CFLAGS) -c as_gen_tones.c -o as_gen_tones.o
	$(CC) -o $@ $^ $(LDFLAGS) -lm
	$(STRIP)  $@
	$(COPY) $@ $(BIN_DIR)


# LDFLAGS+=-L$(BIN_DIR) 

LOCAL_LAST=program

$(LOCAL_LAST):$(OBJS) gen_tones_$(ARCH) gen_dtmf_$(ARCH) #as_zone_tones.h as_dtmf_tones.h


LOCAL_CLEAN:=clean 

$(LOCAL_CLEAN):
	rm -f *.o *.~ *.*~
	for n in $(DELETE_LIST);do \
		if test -f "$$n"; then \
			$(RM) "$$n";\
	  else :; fi; \
	done

