#!/bin/sh
# 为Flash根文件系统安装现有已经发布的、在FlashPreReleases目录下的文件

SRCDIR=$2
DATDIR=`echo $1 | sed 's/\/\//\//g'`

	echo ""
	echo "STEP 2 :"
	echo "         Installing Pre-Releases in Directory of $SRCDIR into $DATDIR for Flash Root File System"
	
	for pdir in $SRCDIR; do
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
The Pre-Releases files of Flash Root File System have been installed. 
======================================================================
EOF

exit 0
