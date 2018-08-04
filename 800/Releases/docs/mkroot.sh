#/bin/sh
# Script for CompactFlash card root file system
# 2004.12.29 build by Lizhijie 

IXP_HOME=`pwd`
COMPACT_FLASH_ROOT_HOME=$IXP_HOME/releases


export IXP_HOME COMPACT_FLASH_ROOT_HOME IMAGE_SIZE OUTPUT_FILE COMPACT_FLASH_ROOT_BACKUP

echo "root directory : $IXP_HOME"

rm -r -f $COMPACT_FLASH_ROOT_HOME

echo "############################################################"
echo "STEP 1 : Prepare Device Driver Files"
echo "############################################################"
rm -r -f $COMPACT_FLASH_ROOT_HOME/dev/
mkdir -p $COMPACT_FLASH_ROOT_HOME/dev/

mknod $COMPACT_FLASH_ROOT_HOME/dev/ram0 	b 1 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/ram1 	b 1 1

mknod $COMPACT_FLASH_ROOT_HOME/dev/mem 	c 1 1
mknod $COMPACT_FLASH_ROOT_HOME/dev/kmem 	c 1 2
mknod $COMPACT_FLASH_ROOT_HOME/dev/null 	c 1 3
mknod $COMPACT_FLASH_ROOT_HOME/dev/zero 	c 1 4

mknod $COMPACT_FLASH_ROOT_HOME/dev/random 	c 1 8
mknod $COMPACT_FLASH_ROOT_HOME/dev/urandom c 1 9

mknod $COMPACT_FLASH_ROOT_HOME/dev/ptyp0   c 2 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/ttyp0   c 3 0  

mknod $COMPACT_FLASH_ROOT_HOME/dev/tty0   c 4 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/tty1   c 4 1

mknod $COMPACT_FLASH_ROOT_HOME/dev/ttyS0 	c 4 64
mknod $COMPACT_FLASH_ROOT_HOME/dev/ttyS1 	c 4 65

mknod $COMPACT_FLASH_ROOT_HOME/dev/tty 	c 5 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/console c 5 1

mknod $COMPACT_FLASH_ROOT_HOME/dev/ppp     c 108 1

mknod $COMPACT_FLASH_ROOT_HOME/dev/mtd0	c 90 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdr0	c 90 1
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtd1	c 90 2
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdr1	c 90 3
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtd2	c 90 4
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdr2	c 90 5
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtd3	c 90 6
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdr3	c 90 7

mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdblock0	b 31 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdblock1	b 31 1
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdblock2	b 31 2
mknod $COMPACT_FLASH_ROOT_HOME/dev/mtdblock3	b 31 3


mknod $COMPACT_FLASH_ROOT_HOME/dev/hda b 3 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda1 b 3 1
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda2 b 3 2
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda3 b 3 3
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda4 b 3 4
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda5 b 3 5
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda6 b 3 6
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda7 b 3 7
mknod $COMPACT_FLASH_ROOT_HOME/dev/hda8 b 3 8

mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb b 3  64
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb1 b 3 65
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb2 b 3 66
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb3 b 3 67
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb4 b 3 68
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb5 b 3 69
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb6 b 3 70
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb7 b 3 71
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdb8 b 3 72

mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc b 22 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc1 b 22 1
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc2 b 22 2
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc3 b 22 3
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc4 b 22 4
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc5 b 22 5
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc6 b 22 6
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc7 b 22 7
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdc8 b 22 8

mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd b 22  64
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd1 b 22 65
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd2 b 22 66
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd3 b 22 67
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd4 b 22 68
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd5 b 22 69
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd6 b 22 70
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd7 b 22 71
mknod $COMPACT_FLASH_ROOT_HOME/dev/hdd8 b 22 72

MAJOR=197

# Global control device file
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/span c $MAJOR 0
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/1 c $MAJOR 1
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/2 c $MAJOR 2
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/3 c $MAJOR 3
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/4 c $MAJOR 4
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/5 c $MAJOR 5
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/6 c $MAJOR 6
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/7 c $MAJOR 7
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/8 c $MAJOR 8
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/9 c $MAJOR 9
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/10 c $MAJOR 10
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/11 c $MAJOR 11
mknod $COMPACT_FLASH_ROOT_HOME/dev/asstel/12 c $MAJOR 12

echo "End of create device files!"
echo ""


echo "############################################################"
echo "STEP 2 : Prepare Files"
echo "############################################################"
mkdir -p $COMPACT_FLASH_ROOT_HOME/lib

mkdir -p $COMPACT_FLASH_ROOT_HOME/var
chmod -R 777  $COMPACT_FLASH_ROOT_HOME/var

mkdir -p $COMPACT_FLASH_ROOT_HOME/tmp
chmod -R 777  $COMPACT_FLASH_ROOT_HOME/tmp

mkdir -p $COMPACT_FLASH_ROOT_HOME/proc
chmod -R 777  $COMPACT_FLASH_ROOT_HOME/proc

mkdir -p $COMPACT_FLASH_ROOT_HOME/mnt
chmod -R 777  $COMPACT_FLASH_ROOT_HOME/mnt

mkdir -p $COMPACT_FLASH_ROOT_HOME/home/root
chmod -R 777  $COMPACT_FLASH_ROOT_HOME/home/root

mkdir -p $COMPACT_FLASH_ROOT_HOME/home/lzj
chmod -R 777  $COMPACT_FLASH_ROOT_HOME/home/lzj

rm -r -f $IXP_HOME/$OUTPUT_FILE


echo "clean and expand new busybox into root fs directory....." 
rm -r -f $COMPACT_FLASH_ROOT_HOME/bin $COMPACT_FLASH_ROOT_HOME/sbin $COMPACT_FLASH_ROOT_HOME/usr/bin $COMPACT_FLASH_ROOT_HOME/usr/sbin 

tar xvf $IXP_HOME/assist_busybox*.tar -C $COMPACT_FLASH_ROOT_HOME/
chmod 755 $COMPACT_FLASH_ROOT_HOME/bin/busybox
echo "End of extract busybox!"
echo ""

echo "############################################################"
echo "STEP 3 : extracting libraries, modules and some binaries"
echo "############################################################"
rm -r -f $COMPACT_FLASH_ROOT_HOME/lib
tar xvzf jffs2_releases*.tar.gz  -C $COMPACT_FLASH_ROOT_HOME/
echo "End of extract libraries!"
echo ""

# echo "copy lrz and lsz to bin directory....." 
cp -r $COMPACT_FLASH_ROOT_BACKUP/* $COMPACT_FLASH_ROOT_HOME/

# echo "copy DHCP client script....." 
# mkdir -p $COMPACT_FLASH_ROOT_HOME/usr/share/udhcpc/
# cp $IXP_HOME/default.script $COMPACT_FLASH_ROOT_HOME/usr/share/udhcpc/
# chmod 755 $COMPACT_FLASH_ROOT_HOME/usr/share/udhcpc/

chown -R root.root $COMPACT_FLASH_ROOT_HOME

echo "############################################################"
echo "STEP 3 : Build COMPACT_FLASH tar.gz file"
echo "############################################################"


echo ""
echo "sucessed now--`date`!" 


