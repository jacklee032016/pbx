#!/bin/sh
# 将安装完成的NTP软件目录打包、发布
# 李志杰 2006.07.09
#

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`
PKGDIR="$DATDIR/../pkg"

	echo ""
	echo ""
	echo "Install AS600 PBX....."
	
	mkdir -p $PKGDIR/usr/bin
	mkdir -p $PKGDIR/lib
	
	echo ""
	echo "Copy Libraries ..."
#		PACKAGES=`find $pdir -name ?.tar.gz `
	LIBS=`find $DATDIR -name lib*.so -prune `
	for p in $LIBS; do
			f=`basename $p`
			echo "   Copy Library $f..."
			cp $p $PKGDIR/lib/
	done

	echo ""
	echo "Copy EXEs ..."
	EXES=`find $DATDIR -name pbx -prune `
	for p in $EXES; do
			f=`basename $p`
			echo "   Copy EXE $f..."
			cp $p $PKGDIR/usr/bin/
	done

	echo "Copy Configuration and Audio Files"
	cp -r $SRCDIR/* $PKGDIR/
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
AS600 PBX installed 
     _________________________________________________________
     *****  $ASSIST_RELEASES_NAME  *****
     ---------------------------------------------------------
              has been build in $BUILDTIME!
Please Check it with Your Board!
======================================================================
EOF

exit 0


