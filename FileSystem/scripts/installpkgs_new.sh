#!/bin/sh
# ��װ�Ѿ����������� assist_****.tar.gz
# ��װ���й����������������ƽ̨���÷ֱ�װAS600��AS800�������
#����־�ܡ�2006.07.07
# 

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`

	AS600_BASE_MODULES=""
	COMMON_BASE_MODULES=""
	AS600_IPPBX_MODULES=""
	COMMON_IPPBX_MOSULES=""
	AS600_SERVICES_MODULES=""

	echo ""
	echo ""
	echo "STEP 3 :"
	echo "         Installing Packages in Directory of $SRCDIR into $DATDIR for platform $PLATFORM"

	cd $DATDIR
	SRCDIRS="$SRCDIR/common $SRCDIR/$PLATFORM"
	
	for pdir in $SRCDIRS; do
		echo "            Extract Packages in $pdir..."
#		PACKAGES=`find $pdir -name ?.tar.gz `
		PACKAGES=`find $pdir -type f | grep tar.gz `
		
		for p in $PACKAGES; do
			f=`basename $p`
			echo "               Extract Package $f..."
			tar xzf $p
			echo "                         Ended!"
			echo ""
		done
	done
	
	cd $DATDIR
	
	tar czf $ASSIST_RELEASES_NAME *
	mv $ASSIST_RELEASES_NAME $ROOT_DIR

cat << EOF
======================================================================
$PLATFORM Released FileSystem Packages 
     _________________________________________________________
     *****  $ASSIST_RELEASES_NAME  *****
     ---------------------------------------------------------
              has been build in $BUILDTIME!
Please Check it with Your Board!
======================================================================
EOF

exit 0

