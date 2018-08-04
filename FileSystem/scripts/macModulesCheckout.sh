#!/bin/sh
# Check out所有其他要备份的无线MAC模块
#　李志杰　2006.12.01
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo ""
	echo "  Checkout Wireless MAC modules for backup....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	MAC_MODULES="AODV Bridge Docs DSR HAODV HostAP Madwifi_Raw MAODV
	      MESH SoftMAC VirtulMAC VMAC_2 Wifi WTools"	

	declare -i ixpCount=0
	
	for plib in $MAC_MODULES; do
		echo "    Checking out Wireless MAC Module \"$plib\"....."
		cvs co MAC_$plib 2>/dev/null 1>/dev/null
		echo ""
		ixpCount=ixpCount+1
	done

#	export CVSROOT=:pserver:lizhijie@192.168.3.166:/CVS/mesh  
#	echo -e "\nlicd001\n" | cvs login >> /dev/null
	
cat << EOF
======================================================================
 Wireless MAC Modules :
     "$MAC_MODULES" ------- $ixpCount Modules
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
