#!/bin/sh
# Check out所有的公共模块
#　李志杰　2006.07.10
# 

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`


	echo ""
	echo ""
	echo "  Checkout WMN Modules....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	. $ROOT_DIR/scripts/wmnModules.list

	mkdir -p $BUILDDIR
	cd $BUILDDIR

	declare -i progCount=0
	
	for prog in $WMN_MODULES; do
		echo "    Checking out WMN Module \"$prog\"....."
		cvs co wmn_$prog 2>/dev/null 1>/dev/null
		echo ""
		progCount=progCount+1
	done

cat << EOF
======================================================================
 WMN Modules :
     "$WMN_MODULES" ------- $progCount Modules
 Packages WMN for WMN Project has been installed. 
======================================================================
EOF

exit 0
