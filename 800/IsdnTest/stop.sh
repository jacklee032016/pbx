# /bin/sh
echo "unload ISDN drivers"
echo "unload ISDN HFC-4S HW Driver..."
rmmod hfc
echo "unload ISDN Layer 3(Japan INS User) driver..."
rmmod ins
echo "unload ISDN Layer 2 driver..."
rmmod kl2
echo "unload ISDN Layer 1 driver..."
rmmod kl1
echo "unload ISDN core engine..."
rmmod isdn
