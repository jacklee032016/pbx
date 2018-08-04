#/bin/sh
# not call it directly
# script for device files 
# Li Zhijie 2004.12.31

rm -r -f $RELEASES_ROOT/dev/
mkdir -p $RELEASES_ROOT/dev/

mknod $RELEASES_ROOT/dev/ram0 	b 1 0
mknod $RELEASES_ROOT/dev/ram1 	b 1 1

mknod $RELEASES_ROOT/dev/mem 	c 1 1
mknod $RELEASES_ROOT/dev/kmem 	c 1 2
mknod $RELEASES_ROOT/dev/null 	c 1 3
mknod $RELEASES_ROOT/dev/zero 	c 1 4

mknod $RELEASES_ROOT/dev/random 	c 1 8
mknod $RELEASES_ROOT/dev/urandom 	c 1 9

mknod $RELEASES_ROOT/dev/ptyp0   c 2 0
mknod $RELEASES_ROOT/dev/ttyp0   c 3 0  

mknod $RELEASES_ROOT/dev/tty0   c 4 0
mknod $RELEASES_ROOT/dev/tty1   c 4 1

mknod $RELEASES_ROOT/dev/ttyS0 	c 4 64
mknod $RELEASES_ROOT/dev/ttyS1 	c 4 65

mknod $RELEASES_ROOT/dev/tty 	c 5 0
mknod $RELEASES_ROOT/dev/console c 5 1

mknod $RELEASES_ROOT/dev/ppp     c 108 1

mknod $RELEASES_ROOT/dev/mtd0	c 90 0
mknod $RELEASES_ROOT/dev/mtdr0	c 90 1
mknod $RELEASES_ROOT/dev/mtd1	c 90 2
mknod $RELEASES_ROOT/dev/mtdr1	c 90 3
mknod $RELEASES_ROOT/dev/mtd2	c 90 4
mknod $RELEASES_ROOT/dev/mtdr2	c 90 5
mknod $RELEASES_ROOT/dev/mtd3	c 90 6
mknod $RELEASES_ROOT/dev/mtdr3	c 90 7

mknod $RELEASES_ROOT/dev/mtdblock0	b 31 0
mknod $RELEASES_ROOT/dev/mtdblock1	b 31 1
mknod $RELEASES_ROOT/dev/mtdblock2	b 31 2
mknod $RELEASES_ROOT/dev/mtdblock3	b 31 3


mknod $RELEASES_ROOT/dev/hda b 3 0
mknod $RELEASES_ROOT/dev/hda1 b 3 1
mknod $RELEASES_ROOT/dev/hda2 b 3 2
mknod $RELEASES_ROOT/dev/hda3 b 3 3
mknod $RELEASES_ROOT/dev/hda4 b 3 4
mknod $RELEASES_ROOT/dev/hda5 b 3 5
mknod $RELEASES_ROOT/dev/hda6 b 3 6
mknod $RELEASES_ROOT/dev/hda7 b 3 7
mknod $RELEASES_ROOT/dev/hda8 b 3 8

mknod $RELEASES_ROOT/dev/hdb b 3  64
mknod $RELEASES_ROOT/dev/hdb1 b 3 65
mknod $RELEASES_ROOT/dev/hdb2 b 3 66
mknod $RELEASES_ROOT/dev/hdb3 b 3 67
mknod $RELEASES_ROOT/dev/hdb4 b 3 68
mknod $RELEASES_ROOT/dev/hdb5 b 3 69
mknod $RELEASES_ROOT/dev/hdb6 b 3 70
mknod $RELEASES_ROOT/dev/hdb7 b 3 71
mknod $RELEASES_ROOT/dev/hdb8 b 3 72

mknod $RELEASES_ROOT/dev/hdc b 22 0
mknod $RELEASES_ROOT/dev/hdc1 b 22 1
mknod $RELEASES_ROOT/dev/hdc2 b 22 2
mknod $RELEASES_ROOT/dev/hdc3 b 22 3
mknod $RELEASES_ROOT/dev/hdc4 b 22 4
mknod $RELEASES_ROOT/dev/hdc5 b 22 5
mknod $RELEASES_ROOT/dev/hdc6 b 22 6
mknod $RELEASES_ROOT/dev/hdc7 b 22 7
mknod $RELEASES_ROOT/dev/hdc8 b 22 8

mknod $RELEASES_ROOT/dev/hdd b 22  64
mknod $RELEASES_ROOT/dev/hdd1 b 22 65
mknod $RELEASES_ROOT/dev/hdd2 b 22 66
mknod $RELEASES_ROOT/dev/hdd3 b 22 67
mknod $RELEASES_ROOT/dev/hdd4 b 22 68
mknod $RELEASES_ROOT/dev/hdd5 b 22 69
mknod $RELEASES_ROOT/dev/hdd6 b 22 70
mknod $RELEASES_ROOT/dev/hdd7 b 22 71
mknod $RELEASES_ROOT/dev/hdd8 b 22 72

MAJOR=197

# Global control device file
mkdir -p $RELEASES_ROOT/dev/asstel
mknod $RELEASES_ROOT/dev/asstel/span c $MAJOR 0
mknod $RELEASES_ROOT/dev/asstel/1 c $MAJOR 1
mknod $RELEASES_ROOT/dev/asstel/2 c $MAJOR 2
mknod $RELEASES_ROOT/dev/asstel/3 c $MAJOR 3
mknod $RELEASES_ROOT/dev/asstel/4 c $MAJOR 4
mknod $RELEASES_ROOT/dev/asstel/5 c $MAJOR 5
mknod $RELEASES_ROOT/dev/asstel/6 c $MAJOR 6
mknod $RELEASES_ROOT/dev/asstel/7 c $MAJOR 7
mknod $RELEASES_ROOT/dev/asstel/8 c $MAJOR 8
mknod $RELEASES_ROOT/dev/asstel/9 c $MAJOR 9
mknod $RELEASES_ROOT/dev/asstel/10 c $MAJOR 10
mknod $RELEASES_ROOT/dev/asstel/11 c $MAJOR 11
mknod $RELEASES_ROOT/dev/asstel/12 c $MAJOR 12

