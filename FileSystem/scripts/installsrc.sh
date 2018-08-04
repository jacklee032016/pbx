#!/bin/sh
# ��װ�����Ѿ������ġ���PreReleasesĿ¼�µ��ļ�

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo "STEP 2 :"
	echo "         Installing Pre-Releases in Directory of $SRCDIR into $DATDIR for platform $PLATFORM"

	SRCDIRS="$SRCDIR/common $SRCDIR/$PLATFORM"
	
	for pdir in $SRCDIRS; do
		echo "         Copy Files in $pdir..."
		cp -R $pdir/* $DATDIR
		echo "             Finished in $pdir!"
		echo ""
	done

	CVS_FILES=`find $DATDIR -name \CVS `
	
	for f in $CVS_FILES; do
#		echo "          Remove $f"
		rm -rf $f
	done
	

cat << EOF
======================================================================
The Pre-Releases files have been installed. 
======================================================================
EOF

exit 0

