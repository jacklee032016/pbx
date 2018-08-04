# /bin/sh
# 这个脚本在安装时调用
# 建立图片的符号连接

LANGS="ja zh_CN.GB2312"

DIRS="/var/admin/cgi-bin /lib /usr/bin"
	
BINDIRNAME=`echo $1 | sed 's/\/\//\//g'`

	for langDir in $LANGS; do
		echo "build symbol link for $langDir in Samba"
		cd $1/var/admin;
		mkdir -p $langDir/swat;
		cd $langDir/swat;
		ln -s ../../swat/images . ;
	done

LANGS="zh_CN.GB2312"

	for langDir in $LANGS; do
		echo "build symbol link for $langDir in CUPS"
		cd $1/var/admin/$langDir/cups;
		if [ -d images ] ; then rm -rf images; fi
		ln -s ../../cups/images . ;
	done

