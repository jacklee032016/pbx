#!/bin/sh
# Check out所有其他要备份的模块
#　李志杰　2006.10.23
# 包括mesh这个存储的模块
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo ""
	echo "  Checkout MESH modules for backup....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	MESH_MODULES="aodv dsr hostap maodv vmac bridge haodv madwifi_raw mesh tools wifi docs"	

#	CVS_PASSFILE=$ROOT_DIR/mesh.cvs

	export CVSROOT=:pserver:lizhijie@192.168.3.166:/CVS/mesh  
#	echo -e "\nlicd001\n" | cvs login >> /dev/null

	declare -i meshCount=0
	
	for plib in $MESH_MODULES; do
		echo "    Checking out MESH Module \"$plib\"....."
		cvs co $plib 2>/dev/null 1>/dev/null
		echo ""
		meshCount=meshCount+1
	done

	
cat << EOF
======================================================================
 MESH Modules :
     "$MESH_MODULES" ------- $meshCount Modules

======================================================================
EOF

exit 0
