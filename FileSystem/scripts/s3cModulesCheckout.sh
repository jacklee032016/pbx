#!/bin/sh
# Check out所有其他要备份芯片S3C 4510B的模块
#　李志杰　2006.12.01
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo ""
	echo "  Checkout S3C 4510B modules for backup....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	S3C_MODULES="bios-lt Busybox ICE Kernel Misc MtdUtils Releases RootFs UCLibc ZLib"	

	declare -i ixpCount=0
	
	for plib in $S3C_MODULES; do
		echo "    Checking out Wireless MAC Module \"$plib\"....."
		cvs co s3c_$plib 2>/dev/null 1>/dev/null
		echo ""
		ixpCount=ixpCount+1
	done

	
cat << EOF
======================================================================
 S3C 4510B Modules :
     "$S3C_MODULES" ------- $ixpCount Modules
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
