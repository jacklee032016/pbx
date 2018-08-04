#/bin/sh
# 创建CompactFlash卡的根文件系统的共享库和需要程序的脚本

RELEASES_DATE=`date +%F`
RELEASES_HOME=`pwd`
RELEASES_ROOT=$RELEASES_HOME/compact_releases


export RELEASES_ROOT

mkdir -p $RELEASES_ROOT
cd $RELEASES_ROOT
echo "############################################################"
echo "STEP 1 : Preparing Files..."
echo "############################################################"
tar xvzf $RELEASES_HOME/assist_common_releases*.tar.gz 

cp -r $RELEASES_HOME/etc . 
cp -r $RELEASES_HOME/usr . 

find . -name \CVS -exec rm -rf {} \;

echo "############################################################"
echo "STEP 2 : Prepare Device Driver Files"
echo "############################################################"
$RELEASES_HOME/mkdev.sh
echo "End of create device files!"
echo ""


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


echo "End of prepare files!"
echo ""	

cd $RELEASES_ROOT

echo "############################################################"
echo "STEP 2 : Extracting the prepared Files..."
echo "############################################################"

echo "End of extract the prepared files!"
echo ""	

echo "############################################################"
echo "STEP 3 : Extracting the build packages..."
echo "############################################################"
echo "**********  STEP 3.1 : Extracting busybox...  *********"
tar xf $RELEASES_HOME/assist_busybox*.tar
echo "**********  STEP 3.2 : Extracting boa...  *********"
tar xf $RELEASES_HOME/assist_boa*.tar
echo "**********  STEP 3.3 : Extracting IDG...  *********"
echo "**********  STEP 3.4 : Extracting pppd...  *********"
tar xf $RELEASES_HOME/assist_pppd*.tar
echo "**********  STEP 3.5 : Extracting pppoe...  *********"
tar xf $RELEASES_HOME/assist_pppoe*.tar
echo "**********  STEP 3.6 : Extracting ftpd...  *********"
tar xf $RELEASES_HOME/assist_vsftpd*.tar
echo "**********  STEP 3.7 : Extracting sip...  *********"
#tar xf $RELEASES_HOME/assist_gua*.tar
echo "**********  STEP 3.8 : Extracting uart...  *********"
tar xf $RELEASES_HOME/assist_ippbx*.tar
echo "**********  STEP 3.9 : Extracting webadmin...  *********"
tar xf $RELEASES_HOME/assist_webadmin*.tar
echo "End of extract the build packages!"
echo ""	

echo "############################################################"
echo "STEP 4 : compressing all releases files..."
echo "############################################################"
tar czf $RELEASES_HOME/assist_compactflash_releases_$RELEASES_DATE.tar.gz *
	
echo "End of compress all releases files!"
echo ""	

rm -r -f $RELEASES_ROOT

echo "CompactFlash root filesystem success on $RELEASES_DATE"
	
