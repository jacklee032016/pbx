#!/bin/sh
# Check out所有其他要备份的模块
#　李志杰　2006.10.23
# 包括mesh这个存储的模块
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo ""
	echo "  Checkout modules for backup....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	IXP_MODULES="as600_Drivers SipServer_All SipServer_Apache SipServer_Java SipServer_Postgre SipServer_Vocal FileSystem common_Sendmail"	

	declare -i ixpCount=0
	
	for plib in $IXP_MODULES; do
		echo "    Checking out other Module \"$plib\"....."
		cvs co $plib 2>/dev/null 1>/dev/null
		echo ""
		ixpCount=ixpCount+1
	done

#	export CVSROOT=:pserver:lizhijie@192.168.3.166:/CVS/mesh  
#	echo -e "\nlicd001\n" | cvs login >> /dev/null
	
cat << EOF
======================================================================
 AS600 Modules :
     "$AS600_MODULES" ------- $as600Count Modules
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
