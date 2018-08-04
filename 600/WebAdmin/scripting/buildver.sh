# /bin/sh
# 创建可执行程序的版本号
# 李志杰 2006.07.12

DATDIR=`echo $1 | sed 's/\/\//\//g'`
#PKGDIR="$DATDIR/pkg"
	
mkdir -p $DATDIR/var/ver
	
VTIME=`date "+%F %T"`

PKGS="webadmin cgipbx"

for VNAME in $PKGS; do

	echo "Create Version Info of $VNAME : "
	VFILE="as_version.h"
	
	if [ $VNAME = "webadmin" ]; then
		cd include 
	else 
		cd modules/sip/cgipbx/
	fi 

	if [ -z $VFILE ]; then 
		echo "No version info found, Skip $p"
	else 		
#		if ($(VFILE),)
		V1TMP=`grep AS_VERSION_MAJOR $VFILE`
		V2TMP=`grep AS_VERSION_MINOR $VFILE`
		V3TMP=`grep AS_VERSION_RELEASE $VFILE`
		V4TMP=`grep AS_VERSION_MODIFY $VFILE`
	
#		echo ""$V1TMP" ..."
		
		V1INDEXTMP=`expr index "$V1TMP" '"'`
		V1INDEX=`expr $V1INDEXTMP + 1`
		V1=`expr substr "$V1TMP" $V1INDEX 2`
		
		V2INDEXTMP=`expr index "$V2TMP" '"'`
		V2INDEX=`expr $V2INDEXTMP + 1`
		V2=`expr substr "$V2TMP" $V2INDEX 1`
		
		V3INDEXTMP=`expr index "$V3TMP" '"'`
		V3INDEX=`expr $V3INDEXTMP + 1`
		V3=`expr substr "$V3TMP" $V3INDEX 2`
		
		V4INDEXTMP=`expr index "$V4TMP" '"'`
		V4INDEX=`expr $V4INDEXTMP + 1`
		V4=`expr substr "$V4TMP" $V4INDEX 2`
	
		echo "$VNAME (version: $V1.$V2.$V3-$V4 $VTIME)" > $DATDIR/var/ver/$VNAME.ver
		echo "            $VNAME (version: $V1.$V2.$V3-$V4 $VTIME)"
		cd ..
	fi
	
done

exit 0
