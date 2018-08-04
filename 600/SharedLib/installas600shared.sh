#!/bin/sh
# 将安装完成的NTP软件目录打包、发布
# 李志杰 2006.07.09
#

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`
PKGDIR="$DATDIR/../pkg"

	echo ""
	echo ""
	echo "Install AS600 Shared Library and EXEs"
	echo "Build Directris in $PKGDIR"
	
	mkdir -p $PKGDIR
	mkdir -p $PKGDIR/lib
	mkdir -p $PKGDIR/usr/bin
#	mkdir -p $PKGDIR/var/ver
	
	echo ""
	echo "   Copy Configuration and Audio Data into $PKGDIR..."
	cp -r $SRCDIR/* $PKGDIR

	echo ""
	echo "   Copy Library into $PKGDIR..."
	EXES=`find $DATDIR -name lib*.so -prune `
	for p in $EXES; do
			f=`basename $p`
			echo "                   $f is copied..."
			cp $p $PKGDIR/lib/
	done

	echo "   Copy EXE into $PKGDIR..."
	EXES=`find $DATDIR/usr/bin/ -type f `
	for p in $EXES; do
			f=`basename $p`
			echo "                  $f is copied..."
			cp $p $PKGDIR/usr/bin
	done

	CVS_FILES=`find $PKGDIR -name \CVS `
	
	for f in $CVS_FILES; do
		echo "          Remove $f"
		rm -rf $f
	done

	cd $PKGDIR
	tar czf $ASSIST_RELEASES_NAME *
	mv $ASSIST_RELEASES_NAME $ROOT_DIR

cat << EOF
======================================================================
AS600 Shared Library and EXEs are installed 
     _________________________________________________________
     *****  $ASSIST_RELEASES_NAME  *****
     ---------------------------------------------------------
              has been build in $BUILDTIME!
Please Check it with Your Board!
======================================================================
EOF

exit 0


