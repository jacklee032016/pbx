#!/bin/sh
# Check out所有、要编译的AS800模块
#　李志杰　2006.12.01
# 四个AS800模块
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`


	echo ""
	echo ""
	echo "  Checkout AS800 modules....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	AS800_MODULES="ICE IsdnTest Kernel PstnAPI PstnTest Releases SharedLib U-boot 
	    VPBX VPBX_Support"	

	declare -i as800Count=0
	
	for plib in $AS800_MODULES; do
		echo "    Checking out AS800 Module \"$plib\"....."
		cvs co as800_$plib 2>/dev/null 1>/dev/null
		echo ""
		as800Count=as800Count+1
	done

cat << EOF
======================================================================
 AS800 Modules :
     "$AS800_MODULES" ------- $as800Count Modules
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
