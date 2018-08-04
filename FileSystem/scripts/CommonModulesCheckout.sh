#!/bin/sh
# Check out所有的公共模块
#　李志杰　2006.07.10
# 

# SRCDIR=$2

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`


	echo ""
	echo ""
	echo "  Checkout Common modules....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
	. $ROOT_DIR/scripts/checklist.sh

	mkdir -p $BUILDDIR
	cd $BUILDDIR
	

	declare -i libCount=0
	declare -i progCount=0
	
	for plib in $COM_LIBS; do
		echo "    Checking out Common Library \"$plib\"....."
		cvs co comLib_$plib 2>/dev/null 1>/dev/null
		echo ""
		libCount=libCount+1
	done

#	COM_PROGS="Boa E2FsProgs ReiserFsUtils FtpServer IpRoute2 IpTables Lrz MtdUtils Ntp PortMap Pppd Ripd RpPPPoE STrace SysKLogd UnRar UpnpIGD"
	
	for prog in $COM_PROGS; do
		echo "    Checking out Common Program \"$prog\"....."
		cvs co common_$prog 2>/dev/null 1>/dev/null
		echo ""
		progCount=progCount+1
	done

cat << EOF
======================================================================
 Common Libraries : 
     "$COM_LIBS" ------- $libCount Libraries
 Common Programs :
     "$COM_PROGS" ------- $progCount Programs
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
