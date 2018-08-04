#/bin/sh
# 2004.10.02 Build root directory for CompactFlash Card

echo ""
echo "Build directory for CompactFlash Card" 
IXP_HOME=/works/ixp_works #`pwd`
RELEASE_SRC_HOME=$IXP_HOME/releases
RELEASE_CF_HOME=$IXP_HOME/releases_cf/rootfs

export IXP_HOME RELEASE_CF_HOME RELEASE_SRC_HOME

echo $IXP_HOME

rm -r -f $RELEASE_CF_HOME
mkdir -p $RELEASE_CF_HOME
mkdir -p $RELEASE_CF_HOME/dev/

mknod $RELEASE_CF_HOME/dev/ram0 	b 1 0
mknod $RELEASE_CF_HOME/dev/ram1 	b 1 1

mknod $RELEASE_CF_HOME/dev/mem 		c 1 1
mknod $RELEASE_CF_HOME/dev/kmem 	c 1 2
mknod $RELEASE_CF_HOME/dev/null 	c 1 3
mknod $RELEASE_CF_HOME/dev/zero 	c 1 4

mknod $RELEASE_CF_HOME/dev/random 	c 1 8
mknod $RELEASE_CF_HOME/dev/urandom 	c 1 9

mknod $RELEASE_CF_HOME/dev/ptyp0   	c 2 0
mknod $RELEASE_CF_HOME/dev/ttyp0   	c 3 0  

mknod $RELEASE_CF_HOME//dev/tty0   	c 4 0
mknod $RELEASE_CF_HOME//dev/tty1   	c 4 1

mknod $RELEASE_CF_HOME/dev/ttyS0 	c 4 64
mknod $RELEASE_CF_HOME/dev/ttyS1 	c 4 65

mknod $RELEASE_CF_HOME/dev/tty 		c 5 0
mknod $RELEASE_CF_HOME/dev/console 	c 5 1

mknod $RELEASE_CF_HOME/dev/ppp     	c 108 1

mknod $RELEASE_CF_HOME/dev/mtd0		c 90 0
mknod $RELEASE_CF_HOME/dev/mtdr0	c 90 1
mknod $RELEASE_CF_HOME/dev/mtd1		c 90 2
mknod $RELEASE_CF_HOME/dev/mtdr1	c 90 3
mknod $RELEASE_CF_HOME/dev/mtd2		c 90 4
mknod $RELEASE_CF_HOME/dev/mtdr2	c 90 5
mknod $RELEASE_CF_HOME/dev/mtd3		c 90 6
mknod $RELEASE_CF_HOME/dev/mtdr3	c 90 7

mknod $RELEASE_CF_HOME/dev/mtdblock0	b 31 0
mknod $RELEASE_CF_HOME/dev/mtdblock1	b 31 1
mknod $RELEASE_CF_HOME/dev/mtdblock2	b 31 2
mknod $RELEASE_CF_HOME/dev/mtdblock3	b 31 3

mknod $RELEASE_CF_HOME/dev/hda		b 3 0
mknod $RELEASE_CF_HOME/dev/hda1		b 3 1
mknod $RELEASE_CF_HOME/dev/hda2		b 3 2
mknod $RELEASE_CF_HOME/dev/hda3		b 3 3

mknod $RELEASE_CF_HOME/dev/hdc		b 22 0
mknod $RELEASE_CF_HOME/dev/hdc1		b 22 1
mknod $RELEASE_CF_HOME/dev/hdc2		b 22 2
mknod $RELEASE_CF_HOME/dev/hdc3		b 22 3

mkdir -p $RELEASE_CF_HOME/lib
mkdir -p $RELEASE_CF_HOME/proc
mkdir -p $RELEASE_CF_HOME/mnt
mkdir -p $RELEASE_CF_HOME/tmp
mkdir -p $RELEASE_CF_HOME/opt

chmod -R 777  $RELEASE_CF_HOME/var

echo ""
echo "Copying files....." 

echo "Copying all configuration files....." 
tar cvf config.tar -C $RELEASE_SRC_HOME/config etc usr var
tar xvf config.tar -C $RELEASE_CF_HOME
rm -r -f config.tar

echo ""
echo "Copying all library files....." 
tar cvf lib.tar -C $RELEASE_SRC_HOME/ lib
tar xvf lib.tar -C $RELEASE_CF_HOME/
rm -r -f lib.tar

tar xvf $RELEASE_SRC_HOME/busybox.tar -C $RELEASE_CF_HOME/
