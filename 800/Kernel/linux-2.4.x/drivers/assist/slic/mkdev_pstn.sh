#
# $Author: lizhijie $
# $Id: mkdev_pstn.sh,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
#
#! /bin/sh
# create all device files need by Assist Telephone card
MAJOR=197

rm -r -f /dev/astel
mkdir -p /dev/astel

# Global control device file
#mknod /dev/astel/span c $MAJOR 0
# Channel 1 device file
mknod /dev/astel/0 c $MAJOR 0
# Channel 2 device file
mknod /dev/astel/1 c $MAJOR 1
# Channel 3 device file
mknod /dev/astel/2 c $MAJOR 2
# Channel 4 device file
mknod /dev/astel/3 c $MAJOR 3
mknod /dev/astel/4 c $MAJOR 4
mknod /dev/astel/5 c $MAJOR 5
mknod /dev/astel/6 c $MAJOR 6
mknod /dev/astel/7 c $MAJOR 7
mknod /dev/astel/8 c $MAJOR 8
mknod /dev/astel/9 c $MAJOR 9
mknod /dev/astel/10 c $MAJOR 10
mknod /dev/astel/11 c $MAJOR 11

