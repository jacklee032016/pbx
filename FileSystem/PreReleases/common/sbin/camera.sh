# /bin/sh
# VIDEO configuration scripts : Add drivers 
# Lizhijie, 2006.11.13
#


CONFIG_VIDEO=/etc/sys/camera.conf
VIDEO_SERVER=/sbin/camserv
VIDEO_RECORD=/sbin/camrecord

var0=0

if [ ! -f "$CONFIG_VIDEO" -o ! -r "$CONFIG_VIDEO" ] ; then
	$ECHO " Some Error in FileSystem, No Video Configuration file is found" >& 2
	exit 1
fi

	. $CONFIG_VIDEO


if [ $VIDEO_ONBOOT = "YES" ]; then
	echo "Start Assist Video SubSystem....."
	insmod videodev
	insmod camera

	while [ "$var0" -lt "$CAMERA_NUM" ]
	do
		echo "No $var0 Camera"        # -n suppresses newline.
		CAMERA_RESOLUTION=$(eval "echo CAMERA_RESOLUTION_$var0")	
		eval CAMERA_RESOLUTION=\$$CAMERA_RESOLUTION
		CAMERA_NETWORK_PORT=$(eval "echo CAMERA_NETWORK_PORT_${var0}")
		eval CAMERA_NETWORK_PORT=\$$CAMERA_NETWORK_PORT
		CAMERA_ONBOOT=$(eval "echo CAMERA_ONBOOT_${var0}")
		eval CAMERA_ONBOOT=\$$CAMERA_ONBOOT

		CAMERA_RECORD=$(eval "echo CAMERA_RECORD_${var0}")
		eval CAMERA_RECORD=\$$CAMERA_RECORD
		CAMERA_FILE_DURATION=$(eval "echo CAMERA_FILE_DURATION_${var0}")
		eval CAMERA_FILE_DURATION=\$$CAMERA_FILE_DURATION
		
		echo "$DEV $STATUS $TYPE $MT $SUBDIRS"
		echo ""
		if [ $CAMERA_ONBOOT = "YES" ]; then
			echo "Start Camara Server $var0, resolution : $CAMERA_RESOLUTION Port : $CAMERA_NETWORK_PORT ....."
			$VIDEO_SERVER -d /dev/video$var0 -s $CAMERA_RESOLUTION -w $CAMERA_NETWORK_PORT &
			
			if [ $CAMERA_RECORD = "YES" ]; then
				if grep -q "/dev/hd" /proc/mounts ; then
				
					CAM_HOME_DIR=$VIDEO_DATA_DIR/cam$var0
					
					if [ ! -d "$CAM_HOME_DIR" ]; then
						mkdir -p $CAM_HOME_DIR
						chmod -R 770 $CAM_HOME_DIR
					fi
					
					sleep 10
					$VIDEO_RECORD -p $CAMERA_FILE_DURATION -w 127.0.0.1:$CAMERA_NETWORK_PORT -o $CAM_HOME_DIR &
				else
					echo "No HD Disk exist, so Camera Recorder is disabled"
				fi	
			fi
			
		else
			echo "Camera Server $var0 is not startup"
		fi
		
		var0=`expr $var0 + 1`
	done
	
else 
	echo " Assist Video SubSystem is not start" >& 2

fi	
