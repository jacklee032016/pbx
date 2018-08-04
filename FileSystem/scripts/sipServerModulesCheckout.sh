#!/bin/sh
# Check out所有其他要备份的SIP Server模块
#　李志杰　2006.12.01
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo ""
	echo "  Checkout SIP Server modules for backup....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	SIP_SERVER_MODULES="All Apache Java Postgre Vocal"	

	declare -i ixpCount=0
	
	for plib in $SIP_SERVER_MODULES; do
		echo "    Checking out SIP Server Module \"$plib\"....."
		cvs co SipServer_$plib 2>/dev/null 1>/dev/null
		echo ""
		ixpCount=ixpCount+1
	done

#	export CVSROOT=:pserver:lizhijie@192.168.3.166:/CVS/mesh  
#	echo -e "\nlicd001\n" | cvs login >> /dev/null
	
cat << EOF
======================================================================
 SIP Server Modules :
     "$SIP_SERVER_MODULES" ------- $ixpCount Modules
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
