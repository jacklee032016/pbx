#!/bin/sh
# 编译所有的公共模块
#　李志杰　2006.07.10
# 

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`
PKGDIR=$2

	echo ""
	echo ""
	echo "  Build Common modules....."
	
	declare -i libCount=0
	declare -i progCount=0
	declare -i pkgCount=0

	. $ROOT_DIR/scripts/checklist.sh
	
	mkdir -p $PKGDIR
	
	cd $BUILDDIR
	
	for plib in $COM_LIBS; do
		echo "    Build Common Library \"$plib\"....."
		cd comLib_$plib
		make > comLib_$plib.log
		echo ""
		libCount=libCount+1
		cd $BUILDDIR
	done
	
	for plib in $COM_LIBS; do
		echo "    Build Common Library \"$plib\"....."
		cd comLib_$plib
		make install > comLib_$plib_install.log
		echo ""
		cd $BUILDDIR
	done
	
	for prog in $COM_PROGS; do
		echo "    Checking out Common Program \"$prog\"....."
		cd common_$prog 
		make > common_$prog.log
		echo ""
		progCount=progCount+1
		cd $BUILDDIR
	done

	for prog in $COM_PROGS; do
		echo "    Checking out Common Program \"$prog\"....."
		cd common_$prog 
		make install > common_install_$prog.log
		echo ""
		cd $BUILDDIR
	done

	PKG_FILES=`find $BUILDDIR -name assist_*.tar.gz -prune `
	
	for p in $PKG_FILES; do
		f=`basename $p`
		echo "          Copy $f to $PKGDIR..."
		cp $p $PKGDIR
		pkgCount=pkgCount+1
	done

cat << EOF
======================================================================
Build Result:
 Common Libraries : 
     "$COM_LIBS" ------- $libCount Libraries
 Common Programs :
     "$COM_PROGS" ------- $progCount Programs
 -------------- total :    $pkgCount Modules installed
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
