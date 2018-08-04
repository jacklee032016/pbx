#/bin/sh
# 安装基本的目录结构和设备文件
# 李志杰 2006.07.07
#

cat << EOF
======================================================================
          Build FileSystem for $PLATFORM from Scratch.
======================================================================
EOF

echo ""
echo ""
echo "STEP 1 :"
echo "        Build Directories for $PLATFORM" 

# while ( test -n "$1" ); do

	echo "                Parent Directory of Device File : $1"

	if [ -d $1 ] ; then rm -rf $1; fi

	mkdir -p $1
	mkdir -p $1/dev/
	
	echo "                Build Miscellious Devices"
	mknod $1/dev/ram0 		b 1 0
	mknod $1/dev/ram1 		b 1 1
	
	mknod $1/dev/mem 		c 1 1
	mknod $1/dev/kmem 		c 1 2
	mknod $1/dev/null 		c 1 3
	mknod $1/dev/zero 		c 1 4
	
	mknod $1/dev/random 	c 1 8
	mknod $1/dev/urandom 	c 1 9
	
	mknod $1/dev/ptyp0   	c 2 0
	mknod $1/dev/ptyp1   	c 2 1
	mknod $1/dev/ptyp2   	c 2 2
	mknod $1/dev/ptyp3   	c 2 3
	mknod $1/dev/ptyp4   	c 2 4
	mknod $1/dev/ptyp5   	c 2 5
	mknod $1/dev/ptyp6   	c 2 6
	
	mknod $1/dev/ttyp0   	c 3 0  
	mknod $1/dev/ttyp1   	c 3 1  
	mknod $1/dev/ttyp2   	c 3 2  
	mknod $1/dev/ttyp3   	c 3 3  
	mknod $1/dev/ttyp4   	c 3 4  
	mknod $1/dev/ttyp5   	c 3 5  
	mknod $1/dev/ttyp6   	c 3 6  
	
	mknod $1/dev/tty0   	c 4 0
	mknod $1/dev/tty1   	c 4 1
	
	mknod $1/dev/ttyS0 		c 4 64
	mknod $1/dev/ttyS1 		c 4 65
	
	mknod $1/dev/tty 		c 5 0
	mknod $1/dev/console 	c 5 1
	
	mknod $1/dev/sysmsg  	c 97  0
	mknod $1/dev/ppp     	c 108 1
	
	echo "                Build MTD Devices"
	mknod $1/dev/mtd0		c 90 0
	mknod $1/dev/mtdr0		c 90 1
	mknod $1/dev/mtd1		c 90 2
	mknod $1/dev/mtdr1		c 90 3
	mknod $1/dev/mtd2		c 90 4
	mknod $1/dev/mtdr2		c 90 5
	mknod $1/dev/mtd3		c 90 6
	mknod $1/dev/mtdr3		c 90 7
	
	mknod $1/dev/mtdblock0	b 31 0
	mknod $1/dev/mtdblock1	b 31 1
	mknod $1/dev/mtdblock2	b 31 2
	mknod $1/dev/mtdblock3	b 31 3
	
	echo "                Build HD Devices"
	mknod $1/dev/hda		b 3 0
	mknod $1/dev/hda1		b 3 1
	mknod $1/dev/hda2		b 3 2
	mknod $1/dev/hda3		b 3 3
	mknod $1/dev/hda4		b 3 4
	mknod $1/dev/hda5		b 3 5
	mknod $1/dev/hda6		b 3 6
	mknod $1/dev/hda7		b 3 7
	mknod $1/dev/hda8		b 3 8
	
	mknod $1/dev/hdb		b 3 64
	mknod $1/dev/hdb1		b 3 65
	mknod $1/dev/hdb2		b 3 66
	mknod $1/dev/hdb3		b 3 67
	mknod $1/dev/hdb4		b 3 68
	mknod $1/dev/hdb5		b 3 69
	mknod $1/dev/hdb6		b 3 70
	mknod $1/dev/hdb7		b 3 71
	mknod $1/dev/hdb8		b 3 72
	
	mknod $1/dev/hdc		b 22 0
	mknod $1/dev/hdc1		b 22 1
	mknod $1/dev/hdc2		b 22 2
	mknod $1/dev/hdc3		b 22 3
	mknod $1/dev/hdc4		b 22 4
	mknod $1/dev/hdc5		b 22 5
	mknod $1/dev/hdc6		b 22 6
	mknod $1/dev/hdc7		b 22 7
	mknod $1/dev/hdc8		b 22 8
	
	mknod $1/dev/hdd		b 22 64
	mknod $1/dev/hdd1		b 22 65
	mknod $1/dev/hdd2		b 22 66
	mknod $1/dev/hdd3		b 22 67
	mknod $1/dev/hdd4		b 22 68
	mknod $1/dev/hdd5		b 22 69
	mknod $1/dev/hdd6		b 22 70
	mknod $1/dev/hdd7		b 22 71
	mknod $1/dev/hdd8		b 22 72
	
	mknod $1/dev/hde		b 33 0
	mknod $1/dev/hde1		b 33 1
	mknod $1/dev/hde2		b 33 2
	mknod $1/dev/hde3		b 33 3
	mknod $1/dev/hde4		b 33 4
	mknod $1/dev/hde5		b 33 5
	mknod $1/dev/hde6		b 33 6
	mknod $1/dev/hde7		b 33 7
	mknod $1/dev/hde8		b 33 8
	mknod $1/dev/hdf		b 33 64
	mknod $1/dev/hdf1		b 33 65
	mknod $1/dev/hdf2		b 33 66
	mknod $1/dev/hdf3		b 33 67
	mknod $1/dev/hdf4		b 33 68
	mknod $1/dev/hdf5		b 33 69
	mknod $1/dev/hdf6		b 33 70
	mknod $1/dev/hdf7		b 33 71
	mknod $1/dev/hdf8		b 33 72

	mknod $1/dev/hdg		b 34 0
	mknod $1/dev/hdg1		b 34 1
	mknod $1/dev/hdg2		b 34 2
	mknod $1/dev/hdg3		b 34 3
	mknod $1/dev/hdg4		b 34 4
	mknod $1/dev/hdg5		b 34 5
	mknod $1/dev/hdg6		b 34 6
	mknod $1/dev/hdg7		b 34 7
	mknod $1/dev/hde8		b 34 8
	mknod $1/dev/hdh		b 34 64
	mknod $1/dev/hdh1		b 34 65
	mknod $1/dev/hdh2		b 34 66
	mknod $1/dev/hdh3		b 34 67
	mknod $1/dev/hdh4		b 34 68
	mknod $1/dev/hdh5		b 34 69
	mknod $1/dev/hdh6		b 34 70
	mknod $1/dev/hdh7		b 34 71
	mknod $1/dev/hdh8		b 34 72


	echo "                Build USB Flash Disk Devices"
	mknod $1/dev/sda		b 8 0
	mknod $1/dev/sda1		b 8 1
	mknod $1/dev/sda2		b 8 2
	mknod $1/dev/sda3		b 8 3
	mknod $1/dev/sda4		b 8 4
	mknod $1/dev/sda5		b 8 5
	mknod $1/dev/sda6		b 8 6
	mknod $1/dev/sda7		b 8 7
	mknod $1/dev/sda8		b 8 8
	
	mknod $1/dev/sdb		b 8 16
	mknod $1/dev/sdb1		b 8 17
	mknod $1/dev/sdb2		b 8 18
	mknod $1/dev/sdb3		b 8 19
	mknod $1/dev/sdb4		b 8 20
	mknod $1/dev/sdb5		b 8 21
	mknod $1/dev/sdb6		b 8 22
	mknod $1/dev/sdb7		b 8 23
	mknod $1/dev/sdb8		b 8 24
	
	mknod $1/dev/sdc		b 8 32
	mknod $1/dev/sdc1		b 8 33
	mknod $1/dev/sdc2		b 8 34
	mknod $1/dev/sdc3		b 8 35
	mknod $1/dev/sdc4		b 8 36
	mknod $1/dev/sdc5		b 8 37
	mknod $1/dev/sdc6		b 8 38
	mknod $1/dev/sdc7		b 8 39
	mknod $1/dev/sdc8		b 8 40
	
	mknod $1/dev/sdd		b 8 48
	mknod $1/dev/sdd1		b 8 49
	mknod $1/dev/sdd2		b 8 50
	mknod $1/dev/sdd3		b 8 51
	mknod $1/dev/sdd4		b 8 52
	mknod $1/dev/sdd5		b 8 53
	mknod $1/dev/sdd6		b 8 54
	mknod $1/dev/sdd7		b 8 55
	mknod $1/dev/sdd8		b 8 56
	
	mknod $1/dev/sde		b 8 64
	mknod $1/dev/sde1		b 8 65
	mknod $1/dev/sde2		b 8 66
	mknod $1/dev/sde3		b 8 67
	mknod $1/dev/sde4		b 8 68
	mknod $1/dev/sde5		b 8 69
	mknod $1/dev/sde6		b 8 70
	mknod $1/dev/sde7		b 8 71
	mknod $1/dev/sde8		b 8 72

	echo "                Build USB Serial Port Devices"
	mknod $1/dev/ttyUSB0		c 188 0
	mknod $1/dev/ttyUSB1		c 188 1
	mknod $1/dev/ttyUSB2		c 188 2
	mknod $1/dev/ttyUSB3		c 188 3
	mknod $1/dev/ttyUSB4		c 188 4
	mknod $1/dev/ttyUSB5		c 188 5
	mknod $1/dev/ttyUSB6		c 188 6
	mknod $1/dev/ttyUSB7		c 188 7

# socket类型的文件不能被tar打包	
# socket文件在启动时，被程序自动创建
#	echo "                Build Socket/Named Pipe of /dev/log"
#	mksock $1/dev/log
#	chmod 777 $1/dev/log
	
	# 创建USB的打印设备，李志杰 2006.06.29
	
	# 必须使nobody用户也可以访问设备
	echo "        Build USB Printer Devices"
	mkdir -p 	$1/dev/usb
	chmod 755	$1/dev/usb
	mknod 		$1/dev/usb/lp0 		c 180 0
	chmod 777 	$1/dev/usb/lp0
	mknod 		$1/dev/usb/lp1 		c 180 1
	chmod 777 	$1/dev/usb/lp1
	
	
	echo "                Build Device Files for Assist Drivers"
	mkdir -p $1/dev/astel
	
	# Global control device file
	echo "                Build PSTN Device Files"
	mknod $1/dev/astel/0 		c 197 0
	mknod $1/dev/astel/1 		c 197 1
	mknod $1/dev/astel/2 		c 197 2
	mknod $1/dev/astel/3 		c 197 3
	mknod $1/dev/astel/4 		c 197 4
	mknod $1/dev/astel/5 		c 197 5
	mknod $1/dev/astel/6 		c 197 6
	mknod $1/dev/astel/7 		c 197 7
	mknod $1/dev/astel/8 		c 197 8
	mknod $1/dev/astel/9 		c 197 9
	mknod $1/dev/astel/10 		c 197 10
	mknod $1/dev/astel/11 		c 197 11
	
	echo "                Build ISDN Device Files"
	echo "                    Build Core ISDN Device for All Ports"
	mknod $1/dev/astel/isdn 	c 198 127
	echo "                    make RAW Device of B Channels in Port A"
	mknod $1/dev/astel/128 		c 198 128
	mknod $1/dev/astel/129 		c 198 129
	echo "                    make RAW Device of B Channels in Port B"
	mknod $1/dev/astel/130 		c 198 130
	mknod $1/dev/astel/131 		c 198 131
	echo "                    make RAW Device of B Channels in Port C"
	mknod $1/dev/astel/132 		c 198 132
	mknod $1/dev/astel/133 		c 198 133
	echo "                    make RAW Device of B Channels in Port D"
	mknod $1/dev/astel/134 		c 198 134
	mknod $1/dev/astel/135 		c 198 135


	MESH_MAJOR=199  
	echo "make device file for Mesh Device"

	MESH_DEV_HOME=$1/dev/mesh

	if [ -d $MESH_DEV_HOME ]; then
		rm -rf $MESH_DEV_HOME
	fi
	mkdir -p $MESH_DEV_HOME
	
	echo "make Mgr device for all Mesh Device"
	mknod $MESH_DEV_HOME/0 	c 	$MESH_MAJOR 0
	
	echo "make Mesh Device For Different MESH DEVICE"
	mknod $MESH_DEV_HOME/1 	c 	$MESH_MAJOR 1
	mknod $MESH_DEV_HOME/2 	c 	$MESH_MAJOR 2
	mknod $MESH_DEV_HOME/3 	c 	$MESH_MAJOR 3
	mknod $MESH_DEV_HOME/4 	c 	$MESH_MAJOR 4

# add video device(v4l) 2006.11.13
	VIDEO_MAJOR=81  
	echo "make device file for Video Device(v4l)"
	for vdev in 0 1 2 3; do
		echo "$1/dev/video$vdev: char $VIDEO_MAJOR $vdev"
		rm -f $1/dev/video$vdev
		mknod $1/dev/video$vdev c $VIDEO_MAJOR $vdev
		chmod 666 $1/dev/video$vdev
	done

# add SCSI generic device 2007.01.28
	SCSI_MAJOR=21
	SCSI_DEVICE_NUMBER=32
	var0=0
	
	echo ""
	echo "make device file for SCSI Generic Device"
	while [ "$var0" -lt "$SCSI_DEVICE_NUMBER" ]
	do
		echo "$1/dev/sg$var0: char $SCSI_MAJOR $var0"
		rm -f $1/dev/sg$var0
		mknod $1/dev/sg$var0 c $SCSI_MAJOR $var0
		chmod 666 $1/dev/sg$var0
		var0=`expr $var0 + 1`
	done

# add SCSI CD-ROM device 2007.01.28
	SCSI_MAJOR=11
	SCSI_DEVICE_NUMBER=4
	var0=0
	
	echo ""
	echo "make device file for SCSI CD-ROM Device"
	while [ "$var0" -lt "$SCSI_DEVICE_NUMBER" ]
	do
		echo "$1/dev/sr$var0: block $SCSI_MAJOR $var0"
		rm -f $1/dev/sr$var0
		mknod $1/dev/sr$var0 b $SCSI_MAJOR $var0
		chmod 666 $1/dev/sr$var0
		var0=`expr $var0 + 1`
	done


	mkdir -p $1/lib
	mkdir -p $1/proc
	mkdir -p $1/mnt
	mkdir -p $1/tmp
	mkdir -p $1/opt
	mkdir -p $1/etc

	
	users="root ftp lizhijie dovecot sky guest1 guest2 guest3 guest4 guest5 
	   guest6 guest7 guest8 guest9 guest10
	   guest11 guest12 guest13 guest14 guest15
	   guest16 guest17 guest18 guest19 guest20 admin"
	for u in $users; do
		mkdir -p $1/home/$u
		chown -R $u.$u $1/home/$u
		mkdir -p $1/var/mail/$u
		chown -R $u.$u $1/var/mail/$u
	done

	mkdir -p $1/var/data/share
	chmod -R 775  $1/var/data/share
	chown -R admin.root $1/var/data/share

# 2007.11.15 Added for common shared folder without authentication 	
	mkdir -p $1/var/data/share/shared
	chmod -R 775  $1/var/data/share/shared
	
	users="guest1 guest2 guest3 guest4 guest5 
	   guest6 guest7 guest8 guest9 guest10
	   guest11 guest12 guest13 guest14 guest15
	   guest16 guest17 guest18 guest19 guest20 admin"
	for u in $users; do
		mkdir -p $1/var/data/share/$u
		chown -R $u.root $1/var/data/share/$u
		chmod -R 770  $1/var/data/share/$u
	done

	mkdir -p $1/var/spool/mail
	mkdir -p $1/var/spool/smtp
	chmod -R 777 $1/var/spool/mail
	chmod -R 777 $1/var/spool/smtp

	echo "AS600 Chengdu R&D Center Released $BUILDTIME" > $1/etc/version.info 
# done
cat << EOF
======================================================================
Device Files for $PLATFORM platform has been installed.
======================================================================
EOF

exit 0

