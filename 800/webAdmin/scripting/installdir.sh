# /bin/sh
# 这个脚本在编译时调用

DIRS="/var/admin/cgi-bin /var/admin/cgi-bin/system /var/admin/cgi-bin/servers /var/admin/cgi-bin/services /var/admin/cgi-bin/network /var/admin/cgi-bin/video /lib /usr/bin /dev /sbin"
	
	BINDIRNAME=`echo $1 | sed 's/\/\//\//g'`
	
	for directory in $DIRS; do
		echo make dir : $BINDIRNAME$directory;
		mkdir -p $BINDIRNAME$directory
	done

# 创建USB的打印设备，李志杰 2006.06.29
# 由创建文件系统的脚本自动创建，这里不在创建，2006.07.14
# 必须使nobody用户也可以访问设备
# echo "Build USB Printer Devices"
# mkdir -p $BINDIRNAME/dev/usb
# mknod $BINDIRNAME/dev/usb/lp0 c 180 0
# chmod 666 $BINDIRNAME/dev/usb/lp0
# mknod $BINDIRNAME/dev/usb/lp1 c 180 1
# chmod 666 $BINDIRNAME/dev/usb/lp1

