#!/bin/sh
# 创建安装sharedLib软件包
# 李志杰 2006.12.22
#

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`
PKGDIR="$DATDIR/../flash_releases"

	echo ""
	echo ""
	echo "Install AS600 Shared Library and EXEs for FLASH File System"
	echo "Build Directris in $PKGDIR"
	
	mkdir -p $PKGDIR
	mkdir -p $PKGDIR/lib
	mkdir -p $PKGDIR/usr/bin
#	mkdir -p $PKGDIR/var/ver
	
	echo ""
	echo "   Copy Library into $PKGDIR..."
	EXES=`find $DATDIR -name lib*.so -prune `
	for p in $EXES; do
			f=`basename $p`
			echo "                   $f is copied..."
			cp $p $PKGDIR/lib/
	done

#	echo "   Copy EXE into $PKGDIR..."
#	EXES=`find $DATDIR/usr/bin/ -type f `
#	EXES="$DATDIR/usr/bin/selftest "
#	for p in $EXES; do
#			f=`basename $p`
#			echo "                  $f is copied..."
#			cp $p $PKGDIR/usr/bin
#	done

	cd $PKGDIR
	tar czf $ASSIST_RELEASES_NAME_FLASH *
	mv $ASSIST_RELEASES_NAME_FLASH $ROOT_DIR

cat << EOF
======================================================================
AS600 Shared Library and EXEs for FLASH File System are installed 
     _________________________________________________________
     *****  $ASSIST_RELEASES_NAME_FLASH  *****
     ---------------------------------------------------------
              has been build in $BUILDTIME!
Please Check it with Your Board!
======================================================================
EOF

exit 0


