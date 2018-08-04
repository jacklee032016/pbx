#!/bin/sh
# Check out所有其他要备份的杂项模块
#　李志杰　2006.12.01
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo ""
	echo "  Checkout Miscellious modules for backup....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	MISC_MODULES="cvs_config FileSystem"	

	declare -i meshCount=0
	
	for plib in $MISC_MODULES; do
		echo "    Checking out Miscellious Module \"$plib\"....."
		cvs co $plib 2>/dev/null 1>/dev/null
		echo ""
		meshCount=meshCount+1
	done

	
cat << EOF
======================================================================
 Miscellious Modules :
     "$MISC_MODULES" ------- $meshCount Modules

======================================================================
EOF

exit 0
