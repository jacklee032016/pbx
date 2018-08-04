#!/bin/sh
# Check out��������Ҫ���ݵĲ���ģ��
#����־�ܡ�2006.12.01
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo ""
	echo "  Checkout TEST modules for backup....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	TEST_MODULES="CDMA HostAPd XRawTV-20051128 XRawTV-3.95"	

	declare -i meshCount=0
	
	for plib in $TEST_MODULES; do
		echo "    Checking out MESH Module \"$plib\"....."
		cvs co Test_$plib 2>/dev/null 1>/dev/null
		echo ""
		meshCount=meshCount+1
	done

	
cat << EOF
======================================================================
 TEST Modules :
     "$TEST_MODULES" ------- $meshCount Modules

======================================================================
EOF

exit 0
