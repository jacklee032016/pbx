#
# $Author: lizhijie $
# $Log: Makefile,v $
# Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
# AS800 Kernel
#
# Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
# kernel and driver for DUX project
#
# Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
# new drivers for all devices
#
# Revision 1.1.1.5  2004/12/31 08:48:46  fengshikui
# no message
#
# Revision 1.1.1.4  2004/12/31 08:38:04  fengshikui
# no message
#
# Revision 1.1.1.3  2004/12/31 08:23:51  fengshikui
# ÐÞ¸Ä°æ
#
# Revision 1.1.1.2  2004/12/31 08:00:09  fengshikui
# ÐÞ¸Ä°æ
#
# Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
# driver for assist telephone cards Tiger320-Si3210/3050
#
# $Revision: 1.1.1.1 $
#

all: law.o
	cp law.o ../
	
law_objs =	as_tel_law.o


law.o: $(law_objs)
	$(LD) -r -o $@ $(law_objs)

	
as_tel_law.o:as_tel_law.c
	$(HOSTCC) $(KFLAGS) -o $@ -c $<


CFLAGS += -I$(TOP_DIR)/include


clean:
	rm -f *.o 
