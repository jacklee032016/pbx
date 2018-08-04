# /bin/sh
# CD-ROM configuration scripts : Add drivers 
# Lizhijie, 2007.01.28
#


CONFIG_CDROM=/etc/sys/cdrom.conf

var0=0

if [ ! -f "$CONFIG_CDROM" -o ! -r "$CONFIG_CDROM" ] ; then
	$ECHO " Some Error in FileSystem, No CDROM Configuration file is found" >& 2
	exit 1
fi

	. $CONFIG_CDROM


if [ $CDROM_ONBOOT = "YES" ]; then
	echo "Start Assist CD-ROM/RW SubSystem....."
	insmod isofs
	insmod cdrom
	echo ""
	if [ $CDROM_DRIVER = "IDE" ]; then
		insmod ide-cd
	else
		insmod ide-scsi
		insmod sg
		insmod sr_mod
	fi
else 
	echo " Assist CD-ROM/RW SubSystem is not start" >& 2

fi	
