# /bin/sh
# echo "unload ISDN drivers"
# echo "unload ISDN HFC-4S HW Driver..."
rmmod assist_isdn_hfc
# echo "unload ISDN Layer 3(Japan INS User) driver..."
rmmod assist_isdn_ins
# echo "unload ISDN Layer 2 driver..."
rmmod assist_isdn_l2
# echo "unload ISDN Layer 1 driver..."
rmmod assist_isdn_l1
# echo "unload ISDN core engine..."
rmmod assist_isdn_core 
