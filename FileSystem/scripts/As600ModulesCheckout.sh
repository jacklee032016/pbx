#!/bin/sh
# Check out���С�Ҫ�����AS600ģ��
#����־�ܡ�2006.07.10
# �˸�AS600ģ��
# 2007_04_03 ���ӵ�Ƭ����PBX���� NewStc
# 

BUILDDIR=`echo $1 | sed 's/\/\//\//g'`


	echo ""
	echo ""
	echo "  Checkout AS600 modules....."

	if [ -d $BUILDDIR ] ; then rm -rf $BUILDDIR; fi
	
#	AS600_MODULES="Drivers ICE Kernel Pbx SharedLib U-boot VoipLib WebAdmin NewStc"	
	. $ROOT_DIR/scripts/checklist.sh

	mkdir -p $BUILDDIR
	cd $BUILDDIR
	
	declare -i as600Count=0
	
	for plib in $AS600_MODULES; do
		echo "    Checking out AS600 Module \"$plib\"....."
		cvs co as600_$plib 2>/dev/null 1>/dev/null
		echo ""
		as600Count=as600Count+1
	done

cat << EOF
======================================================================
 AS600 Modules :
     "$AS600_MODULES" ------- $as600Count Modules
 Packages common for all platforms has been installed. 
======================================================================
EOF

exit 0
