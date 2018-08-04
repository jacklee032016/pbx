#!/bin/sh
# 编译所有的公共模块
#　李志杰　2006.07.10
# 四个AS600模块

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`
PKGDIR=$2

	echo ""
	echo ""
	echo "  Build AS600 modules....."
	
	declare -i as600Count=0

	mkdir -p $PKGDIR
	
	cd $BUILDDIR
	AS600_MODULES="SharedLib VoipLib Pbx WebAdmin"	
	
	for plib in $AS600_MODULES; do
		echo "    Build AS600 Module \"$plib\"....."
		cd as600_$plib
		make > as600_$plib.log
		echo ""
		cd $BUILDDIR
	done
	
	for plib in $AS600_MODULES; do
		echo "    Build Install Pkg of AS600 Module \"$plib\"....."
		cd as600_$plib
		make install > as600_$plib_install.log
		echo ""
		cd $BUILDDIR
	done
	

	PKG_FILES=`find $BUILDDIR -name as600_*.tar.gz -prune `
	
	for p in $PKG_FILES; do
		f=`basename $p`
		echo "          Copy $f to $PKGDIR..."
		cp $p $PKGDIR
		as600Count=as600Count+1
	done

cat << EOF
======================================================================
Build Result:
 AS600 Modules :
     "$COM_PROGS" 
 -------------- total :    $as600Count AS600 Modules installed
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
