#/bin/sh
# 创建JFFS2的根文件系统的共享库和需要程序的脚本

RELEASES_DATE=`date +%F`
RELEASES_HOME=`pwd`
RELEASES_FLASH=$RELEASES_HOME/flash
RELEASES_ROOT=$RELEASES_FLASH/flash_releases

# 2368K jffs2 root fs partition
IMAGE_SIZE=14942208
OUTPUT_FILE=jffs2.big.image


export RELEASES_ROOT IMAGE_SIZE OUTPUT_FILE

cd $RELEASES_FLASH

echo "Step 1 : get needed files...."
tar xvzf $RELEASES_HOME/assist_common_releases*.tar.gz

tar czf $RELEASES_FLASH/assist_jffs2_releases_$RELEASES_DATE.tar.gz \
	lib/ld* lib/libc.so lib/libc.so.6 lib/libc-2.* lib/libcryp* lib/libz* \
	lib/modules/* \
	bin/ldd bin/lrz bin/lsz bin/strace \
	sbin/lock sbin/unlock sbin/eraseall sbin/network-setup

rm -rf $RELEASES_FLASH/bin
rm -rf $RELEASES_FLASH/sbin
rm -rf $RELEASES_FLASH/lib

	
mkdir -p $RELEASES_ROOT
cd $RELEASES_ROOT
tar xzf $RELEASES_FLASH/assist_jffs2_releases_$RELEASES_DATE.tar.gz 
rm -rf $RELEASES_FLASH/assist_jffs2_releases_$RELEASES_DATE.tar.gz 


echo "############################################################"
echo "STEP 1 : Prepare Device Driver Files"
echo "############################################################"
$RELEASES_HOME/mkdev.sh
echo "End of create device files!"
echo ""

cp -r $RELEASES_FLASH/jffs2_root_back/*  $RELEASES_ROOT

find . -name \CVS -exec rm -rf {} \;

echo "############################################################"
echo "STEP 2 : Prepare Files"
echo "############################################################"
mkdir -p $RELEASES_ROOT/lib

mkdir -p $RELEASES_ROOT/var
chmod -R 777  $RELEASES_ROOT/var

mkdir -p $RELEASES_ROOT/tmp
chmod -R 777  $RELEASES_ROOT/tmp

mkdir -p $RELEASES_ROOT/proc
chmod -R 777  $RELEASES_ROOT/proc

mkdir -p $RELEASES_ROOT/mnt
chmod -R 777  $RELEASES_ROOT/mnt

mkdir -p $RELEASES_ROOT/home/root
chmod -R 777  $RELEASES_ROOT/home/root

mkdir -p $RELEASES_ROOT/home/lzj
chmod -R 777  $RELEASES_ROOT/home/lzj

rm -r -f $RELEASES_ROOT/$OUTPUT_FILE

tar xvf $RELEASES_HOME/assist_busybox*.tar -C $RELEASES_ROOT/
chmod 755 $RELEASES_ROOT/bin/busybox
echo "End of extract busybox!"
echo ""


chown -R root.root $RELEASES_ROOT

echo "############################################################"
echo "STEP 3 : Build JFFS2 image"
echo "############################################################"
# test
tar czf $RELEASES_HOME/assist_jffs2_releases_$RELEASES_DATE.tar.gz * 

cd $RELEASES_FLASH
mkfs.jffs2 --root=$RELEASES_ROOT  --big-endian --output=$OUTPUT_FILE --pad=$IMAGE_SIZE


rm -rf $RELEASES_ROOT

echo "Success on $RELEASES_DATE"
	
