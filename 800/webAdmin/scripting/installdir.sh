# /bin/sh
# ����ű��ڱ���ʱ����

DIRS="/var/admin/cgi-bin /var/admin/cgi-bin/system /var/admin/cgi-bin/servers /var/admin/cgi-bin/services /var/admin/cgi-bin/network /var/admin/cgi-bin/video /lib /usr/bin /dev /sbin"
	
	BINDIRNAME=`echo $1 | sed 's/\/\//\//g'`
	
	for directory in $DIRS; do
		echo make dir : $BINDIRNAME$directory;
		mkdir -p $BINDIRNAME$directory
	done

# ����USB�Ĵ�ӡ�豸����־�� 2006.06.29
# �ɴ����ļ�ϵͳ�Ľű��Զ����������ﲻ�ڴ�����2006.07.14
# ����ʹnobody�û�Ҳ���Է����豸
# echo "Build USB Printer Devices"
# mkdir -p $BINDIRNAME/dev/usb
# mknod $BINDIRNAME/dev/usb/lp0 c 180 0
# chmod 666 $BINDIRNAME/dev/usb/lp0
# mknod $BINDIRNAME/dev/usb/lp1 c 180 1
# chmod 666 $BINDIRNAME/dev/usb/lp1

