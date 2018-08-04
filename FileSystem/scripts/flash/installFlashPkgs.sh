#!/bin/sh
# 生成FLASH根文件系统的映像文件
# 安装所有Flash根文件系统需要的软件包
# 李志杰　2006.12.03
# $Id: installFlashPkgs.sh,v 1.5 2007/01/14 19:32:22 lizhijie Exp $

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`


	echo ""
	echo ""
	echo "STEP 3 :"
	echo "         Installing Packages in Directory of $SRCDIR into $DATDIR for Flash Root File System"

	cd $DATDIR
	
	for pdir in $SRCDIR; do
		echo "            Extract Packages in $pdir..."
		PACKAGES=`find $pdir -type f | grep tar.gz `
		
		for p in $PACKAGES; do
			f=`basename $p`
			echo "               Extract Package $f..."
			tar xzf $p
			echo "                         Ended!"
			echo ""
		done
	done

	cd $ROOT_DIR
	
	chown -R root.root $DATDIR

	echo "############################################################"
	echo "STEP 4 : Build JFFS2 image use $DATDIR as root"
	echo "         Root Directory : $DATDIR"
	echo "         Image Size     : $FLASH_IMAGE_SIZE bytes"
	echo "         Block Size     : $FLASH_ERASE_SIZE bytes"
	echo "############################################################"
#--eraseblock $FLASH_ERASE_SIZE	 
	mkfs.jffs2 --root=$DATDIR  --big-endian --output=$FLASH_OUTPUT_FILE --pad=$FLASH_IMAGE_SIZE
#	mkfs.jffs2 --root=$DATDIR  --little-endian --output=$FLASH_OUTPUT_FILE --pad=$FLASH_IMAGE_SIZE  --eraseblock $FLASH_ERASE_SIZE


cat << EOF
======================================================================
FLASH ROOT FileSystem Released Image
     _________________________________________________________
     *****  $FLASH_OUTPUT_FILE  *****
     ---------------------------------------------------------
              has been build in $BUILDTIME!
Please Check it with Your Board!
======================================================================
EOF

exit 0

