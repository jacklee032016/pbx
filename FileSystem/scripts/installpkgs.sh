#!/bin/sh
# 安装已经编译的软件包 assist_****.tar.gz
# 安装所有公共软件包，并根据平台配置分别安装AS600和AS800的软件包
#　李志杰　2006.07.07
# 

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`


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
	
	echo ""
	echo ""
	echo "STEP 3.A :"
	echo "         some initing tasks before packaging..."

	cd $DATDIR
	mkdir -p $1/var/spool/mail
	mkdir -p $1/var/spool/smtp
	chmod -R 777 $1/var/spool/mail
	chmod -R 777 $1/var/spool/smtp
	
	echo ""
	echo ""
	echo "STEP 4 :"
	echo "         create releases packages in $BUILDTIME..."
	
#	mkdir -p $ROOT_DIR/parts
	# 5 directories for A
#	echo "               create Package $ASSIST_RELEASES_NAME_PART_A..."
#	tar czf $ASSIST_RELEASES_NAME_PART_A arm-linux bin dev etc home
#	mv $ASSIST_RELEASES_NAME_PART_A $ROOT_DIR/parts
	
	# 4 directories for B
#	echo "               create Package $ASSIST_RELEASES_NAME_PART_B..."
#	tar czf $ASSIST_RELEASES_NAME_PART_B lib 
#	mv $ASSIST_RELEASES_NAME_PART_B $ROOT_DIR/parts

	# 3 directories for C
#	echo "               create Package $ASSIST_RELEASES_NAME_PART_C..."
#	tar czf $ASSIST_RELEASES_NAME_PART_C mnt opt proc sbin tmp
#	mv $ASSIST_RELEASES_NAME_PART_C $ROOT_DIR/parts
	
	# 1 directories for D
#	echo "               create Package $ASSIST_RELEASES_NAME_PART_D..."
#	tar czf $ASSIST_RELEASES_NAME_PART_D usr var
#	mv $ASSIST_RELEASES_NAME_PART_D $ROOT_DIR/parts
	
	echo "               create Package $ASSIST_RELEASES_NAME..."
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

