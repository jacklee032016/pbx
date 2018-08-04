# /bin/sh
echo "start ISDN drivers"
echo "load ISDN core engine..."
insmod isdn.o
echo "load ISDN Layer 1 driver..."
insmod kl1.o
echo "load ISDN Layer 2 driver..."
insmod kl2.o
echo "load ISDN Layer 3(Japan INS User) driver..."
insmod ins.o
echo "load ISDN HFC-4S HW Driver..."
insmod hfc.o
