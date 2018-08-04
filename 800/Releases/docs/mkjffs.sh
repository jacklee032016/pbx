#/bin/sh
# Script for JFFS2 root file system
# 2004.12.29 build by Lizhijie 

IXP_HOME=`pwd`

JFFS2_ROOT_HOME=$IXP_HOME/jffs2_root

JFFS2_ROOT_BACKUP=$IXP_HOME/jffs2_root_back

# 2368K jffs2 root fs partition
IMAGE_SIZE=2424832
OUTPUT_FILE=jffs2.big.image

export IXP_HOME JFFS2_ROOT_HOME IMAGE_SIZE OUTPUT_FILE JFFS2_ROOT_BACKUP

echo "root directory : $IXP_HOME"

rm -r -f $JFFS2_ROOT_HOME

echo "############################################################"
echo "STEP 1 : Prepare Device Driver Files"
echo "############################################################"
rm -r -f $JFFS2_ROOT_HOME/dev/
mkdir -p $JFFS2_ROOT_HOME/dev/

mknod $JFFS2_ROOT_HOME/dev/ram0 	b 1 0
mknod $JFFS2_ROOT_HOME/dev/ram1 	b 1 1

mknod $JFFS2_ROOT_HOME/dev/mem 		c 1 1
mknod $JFFS2_ROOT_HOME/dev/kmem 	c 1 2
mknod $JFFS2_ROOT_HOME/dev/null 	c 1 3
mknod $JFFS2_ROOT_HOME/dev/zero 	c 1 4

mknod $JFFS2_ROOT_HOME/dev/random 	c 1 8
mknod $JFFS2_ROOT_HOME/dev/urandom 	c 1 9

mknod $JFFS2_ROOT_HOME/dev/ptyp0   	c 2 0
mknod $JFFS2_ROOT_HOME/dev/ttyp0   	c 3 0  

mknod $JFFS2_ROOT_HOME/dev/tty0   	c 4 0
mknod $JFFS2_ROOT_HOME/dev/tty1   	c 4 1

mknod $JFFS2_ROOT_HOME/dev/ttyS0 	c 4 64
mknod $JFFS2_ROOT_HOME/dev/ttyS1 	c 4 65

mknod $JFFS2_ROOT_HOME/dev/tty 		c 5 0
mknod $JFFS2_ROOT_HOME/dev/console 	c 5 1

mknod $JFFS2_ROOT_HOME/dev/ppp     	c 108 1

mknod $JFFS2_ROOT_HOME/dev/mtd0		c 90 0
mknod $JFFS2_ROOT_HOME/dev/mtdr0	c 90 1
mknod $JFFS2_ROOT_HOME/dev/mtd1		c 90 2
mknod $JFFS2_ROOT_HOME/dev/mtdr1	c 90 3
mknod $JFFS2_ROOT_HOME/dev/mtd2		c 90 4
mknod $JFFS2_ROOT_HOME/dev/mtdr2	c 90 5
mknod $JFFS2_ROOT_HOME/dev/mtd3		c 90 6
mknod $JFFS2_ROOT_HOME/dev/mtdr3	c 90 7

mknod $JFFS2_ROOT_HOME/dev/mtdblock0	b 31 0
mknod $JFFS2_ROOT_HOME/dev/mtdblock1	b 31 1
mknod $JFFS2_ROOT_HOME/dev/mtdblock2	b 31 2
mknod $JFFS2_ROOT_HOME/dev/mtdblock3	b 31 3


mknod $JFFS2_ROOT_HOME/dev/hda b 3 0
mknod $JFFS2_ROOT_HOME/dev/hda1 b 3 1
mknod $JFFS2_ROOT_HOME/dev/hda2 b 3 2
mknod $JFFS2_ROOT_HOME/dev/hda3 b 3 3
mknod $JFFS2_ROOT_HOME/dev/hda4 b 3 4
mknod $JFFS2_ROOT_HOME/dev/hda5 b 3 5
mknod $JFFS2_ROOT_HOME/dev/hda6 b 3 6
mknod $JFFS2_ROOT_HOME/dev/hda7 b 3 7
mknod $JFFS2_ROOT_HOME/dev/hda8 b 3 8

mknod $JFFS2_ROOT_HOME/dev/hdb b 3  64
mknod $JFFS2_ROOT_HOME/dev/hdb1 b 3 65
mknod $JFFS2_ROOT_HOME/dev/hdb2 b 3 66
mknod $JFFS2_ROOT_HOME/dev/hdb3 b 3 67
mknod $JFFS2_ROOT_HOME/dev/hdb4 b 3 68
mknod $JFFS2_ROOT_HOME/dev/hdb5 b 3 69
mknod $JFFS2_ROOT_HOME/dev/hdb6 b 3 70
mknod $JFFS2_ROOT_HOME/dev/hdb7 b 3 71
mknod $JFFS2_ROOT_HOME/dev/hdb8 b 3 72

mknod $JFFS2_ROOT_HOME/dev/hdc b 22 0
mknod $JFFS2_ROOT_HOME/dev/hdc1 b 22 1
mknod $JFFS2_ROOT_HOME/dev/hdc2 b 22 2
mknod $JFFS2_ROOT_HOME/dev/hdc3 b 22 3
mknod $JFFS2_ROOT_HOME/dev/hdc4 b 22 4
mknod $JFFS2_ROOT_HOME/dev/hdc5 b 22 5
mknod $JFFS2_ROOT_HOME/dev/hdc6 b 22 6
mknod $JFFS2_ROOT_HOME/dev/hdc7 b 22 7
mknod $JFFS2_ROOT_HOME/dev/hdc8 b 22 8

mknod $JFFS2_ROOT_HOME/dev/hdd b 22  64
mknod $JFFS2_ROOT_HOME/dev/hdd1 b 22 65
mknod $JFFS2_ROOT_HOME/dev/hdd2 b 22 66
mknod $JFFS2_ROOT_HOME/dev/hdd3 b 22 67
mknod $JFFS2_ROOT_HOME/dev/hdd4 b 22 68
mknod $JFFS2_ROOT_HOME/dev/hdd5 b 22 69
mknod $JFFS2_ROOT_HOME/dev/hdd6 b 22 70
mknod $JFFS2_ROOT_HOME/dev/hdd7 b 22 71
mknod $JFFS2_ROOT_HOME/dev/hdd8 b 22 72

echo "End of create device files!"
echo ""


echo "############################################################"
echo "STEP 2 : Prepare Files"
echo "############################################################"
mkdir -p $JFFS2_ROOT_HOME/lib

mkdir -p $JFFS2_ROOT_HOME/var
chmod -R 777  $JFFS2_ROOT_HOME/var

mkdir -p $JFFS2_ROOT_HOME/tmp
chmod -R 777  $JFFS2_ROOT_HOME/tmp

mkdir -p $JFFS2_ROOT_HOME/proc
chmod -R 777  $JFFS2_ROOT_HOME/proc

mkdir -p $JFFS2_ROOT_HOME/mnt
chmod -R 777  $JFFS2_ROOT_HOME/mnt

mkdir -p $JFFS2_ROOT_HOME/home/root
chmod -R 777  $JFFS2_ROOT_HOME/home/root

mkdir -p $JFFS2_ROOT_HOME/home/lzj
chmod -R 777  $JFFS2_ROOT_HOME/home/lzj

rm -r -f $IXP_HOME/$OUTPUT_FILE


echo "clean and expand new busybox into root fs directory....." 
rm -r -f $JFFS2_ROOT_HOME/bin $JFFS2_ROOT_HOME/sbin $JFFS2_ROOT_HOME/usr/bin $JFFS2_ROOT_HOME/usr/sbin 

tar xvf $IXP_HOME/assist_busybox*.tar -C $JFFS2_ROOT_HOME/
chmod 755 $JFFS2_ROOT_HOME/bin/busybox
echo "End of extract busybox!"
echo ""

echo "############################################################"
echo "STEP 3 : extracting libraries, modules and some binaries"
echo "############################################################"
rm -r -f $JFFS2_ROOT_HOME/lib
tar xvzf jffs2_releases*.tar.gz  -C $JFFS2_ROOT_HOME/
echo "End of extract libraries!"
echo ""

# echo "copy lrz and lsz to bin directory....." 
cp -r $JFFS2_ROOT_BACKUP/* $JFFS2_ROOT_HOME/

# echo "copy DHCP client script....." 
# mkdir -p $JFFS2_ROOT_HOME/usr/share/udhcpc/
# cp $IXP_HOME/default.script $JFFS2_ROOT_HOME/usr/share/udhcpc/
# chmod 755 $JFFS2_ROOT_HOME/usr/share/udhcpc/

chown -R root.root $JFFS2_ROOT_HOME

echo "############################################################"
echo "STEP 3 : Build JFFS2 image"
echo "############################################################"
mkfs.jffs2 --root=$JFFS2_ROOT_HOME  --big-endian --output=$OUTPUT_FILE --pad=$IMAGE_SIZE

echo ""
echo "sucessed now--`date`!" 

