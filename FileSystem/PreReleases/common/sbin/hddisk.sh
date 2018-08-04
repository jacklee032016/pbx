# /bin/sh
# IDE HD Disk mount scripts
# Lizhijie, 2006.08.14
#

MOUNT=/bin/mount
CONFIG_HD_DISK=/etc/sys/hddisk.conf

. $CONFIG_HD_DISK


HDS="hda hdc"
IS_HD_DISK=NO

var0=1
for dev in $HDS; do
	if grep -q "/dev/$dev$var0" /proc/mounts ; then
#		echo "HD Disk $dev$var0"
		IS_HD_DISK=YES
		
		var0=2
#		echo "HD Disk Partition : $dev$var0"
		if grep -q "$dev$var0" /proc/partitions ; then
#			echo "mount $dev$var0 as /var/spool"
		   	mount -t reiserfs /dev/$dev$var0 /var/spool > /dev/null  2>&1
		fi	
		var0=3
#		echo "HD Disk Partition : $dev$var0"
		if grep -q "$dev$var0" /proc/partitions ; then
#			echo "mount $dev$var0 as /var/data/share"
			mount -t reiserfs /dev/$dev$var0 /var/data/share > /dev/null  2>&1
		fi 
	fi	  
done

var0=0
if [ $IS_HD_DISK = "NO" ]; then
	if [ $HD_STATUS = "mount" ]; then
		while [ "$var0" -lt "$HD_NUM" ]
		do
			echo "No $var0 Disk Partition"        # -n suppresses newline.
			DEV=$(eval "echo PART_$var0")	
			eval DEV=\$$DEV
			STATUS=$(eval "echo PART_${var0}_STATUS")
			eval STATUS=\$$STATUS
			TYPE=$(eval "echo PART_${var0}_TYPE")
			eval TYPE=\$$TYPE
			MT=$(eval "echo PART_${var0}_MT")
			eval MT=\$$MT
			SUBDIRS=$(eval "echo PART_${var0}_SUBDIRS")
			eval SUBDIRS=\$$SUBDIRS
			echo "$DEV $STATUS $TYPE $MT $SUBDIRS"
			echo ""
			if [ $STATUS = "mount" ]; then
				if [ ! -d $MT ];then
					echo "First time for HD Disk $DEV Mount"
					mkdir -p $MT
				fi
				
				CMD=$(eval "echo $MOUNT -t $TYPE $DEV $MT")
				echo $CMD
				$CMD
	
				for dir in $SUBDIRS; do
					if [ ! -d $MT/$dir ];then
						echo "mkdir $MT/$dir"
						mkdir -p $MT/$dir
						chmod -R 777 $MT/$dir 
					fi
				done
			else
				echo "Partition $DEV is not format correctly"
			fi
			
			var0=`expr $var0 + 1`
		done
	else 
		echo " HD Disk is not found or format" >& 2
		echo " So no disk is mount now. If you have disk, please check it in WebAdmin" >& 2
	fi
	
fi

