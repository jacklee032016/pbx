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
# Revision 1.1.1.5  2004/12/31 08:48:45  fengshikui
# no message
#
# Revision 1.1.1.4  2004/12/31 08:38:04  fengshikui
# no message
#
# Revision 1.1.1.3  2004/12/31 08:23:51  fengshikui
# �޸İ�
#
# Revision 1.1.1.2  2004/12/31 08:00:09  fengshikui
# �޸İ�
#
# Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
# driver for assist telephone cards Tiger320-Si3210/3050
#
# $Revision: 1.1.1.1 $
#

all: file.o
	cp file.o ../
	
file_objs =	as_dev_file_rw.o as_dev_file_open.o as_dev_file_poll.o 


file.o: $(file_objs)
	$(LD) -r -o $@ $(file_objs)

	
as_dev_file_rw.o:as_dev_file_rw.c
	$(HOSTCC) $(KFLAGS) -o $@ -c $<

as_dev_file_open.o:as_dev_file_open.c
	$(HOSTCC) $(KFLAGS) -o $@ -c $<

as_dev_file_poll.o:as_dev_file_poll.c
	$(HOSTCC) $(KFLAGS) -o $@ -c $<


CFLAGS += -I$(TOP_DIR)/include


clean:
	rm -f *.o 
