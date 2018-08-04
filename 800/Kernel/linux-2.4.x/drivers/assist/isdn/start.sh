# /bin/sh
# echo "start ISDN drivers"
# echo "load ISDN core engine..."
insmod ./assist_isdn_core.o
# echo "load ISDN Layer 1 driver..."
insmod ./assist_isdn_l1.o
# echo "load ISDN Layer 2 driver..."
insmod ./assist_isdn_l2.o
# echo "load ISDN Layer 3(Japan INS User) driver..."
insmod ./assist_isdn_ins.o
# echo "load ISDN HFC-4S HW Driver..."
insmod ./assist_isdn_hfc.o port_used=0x2 port_protocol=0x0 
