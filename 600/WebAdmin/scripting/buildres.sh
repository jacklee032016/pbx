# /bin/sh
# 创建可执行程序的版本号
# 李志杰 2006.07.12

DATDIR=`echo $1 | sed 's/\/\//\//g'`

RES_DIRS=" $DATDIR/common_FileServer $DATDIR/include $DATDIR/lib $DATDIR/modules"

FILE_LC=$DATDIR/filelist.lc

echo "# webadmin source file list" > $FILE_LC
echo "#" >> $FILE_LC

for dir in $RES_DIRS; do

	echo "Create Version Info of $dir : "
	cd $dir; 
	find -name "*.h" -exec ls {} >> $FILE_LC \; 
	find -name "*.c" -exec ls {} >> $FILE_LC \; 
	
done

exit 0
