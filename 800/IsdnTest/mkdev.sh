# /bin/sh
MAJOR=197 

echo "make device file for Assist ISDN driver"

mkdir -p /dev/astel

echo "make core device for all ports"
mknod /dev/astel/isdn c $MAJOR 127

echo "make raw device of B channels in port A"
mknod /dev/astel/128 c $MAJOR 128
mknod /dev/astel/129 c $MAJOR 129
echo "make raw device of B channels in port B"
mknod /dev/astel/130 c $MAJOR 130
mknod /dev/astel/131 c $MAJOR 131
echo "make raw device of B channels in port C"
mknod /dev/astel/132 c $MAJOR 132
mknod /dev/astel/133 c $MAJOR 133
echo "make raw device of B channels in port D"
mknod /dev/astel/134 c $MAJOR 134
mknod /dev/astel/135 c $MAJOR 135
