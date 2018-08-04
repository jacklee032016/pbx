#!/bin/sh
# 完成WebAdmin软件包的发布
# 李志杰 2006.12.22
#

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`
PKGDIR="$DATDIR/../flash_releases"

	echo ""
	echo ""
	echo "Install AS600 Web Admin Modules for FLASH File System....."
	
	mkdir -p $PKGDIR/usr/bin
	mkdir -p $PKGDIR/lib
	mkdir -p $PKGDIR/var/admin/cgi-bin
	
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
	EXES="$DATDIR/usr/bin/setdeft"
	for p in $EXES; do
			f=`basename $p`
			echo "   Copy EXE $f..."
			cp $p $PKGDIR/usr/bin/
	done

	echo ""
	echo "Copy CGIs ..."
	EXES="$DATDIR/var/admin/cgi-bin/wizard.cgi"
	for p in $EXES; do
			f=`basename $p`
			echo "   Copy CGI $f..."
			cp $p $PKGDIR/var/admin/cgi-bin/
	done
	

	cd $PKGDIR
	tar czf $ASSIST_RELEASES_NAME_FLASH *
	mv $ASSIST_RELEASES_NAME_FLASH $ROOT_DIR

cat << EOF
======================================================================
AS600 PBX installed 
     _________________________________________________________
     *****  $ASSIST_RELEASES_NAME_FLASH  *****
     ---------------------------------------------------------
              has been build in $BUILDTIME!
Please Check it with Your Board!
======================================================================
EOF

exit 0


