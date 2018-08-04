# /bin/sh
# WAN configuration scripts : both for MAC and IP address
# Lizhijie, 2006.08.08
#

IFCONFIG=/sbin/ifconfig

CONFIG_HW_ETH0=/etc/sys/hwcfg-eth0
if [ ! -f "$CONFIG_HW_ETH0" -o ! -r "$CONFIG_HW_ETH0" ] ; then
	$ECHO " LAN interface use default hardware interface '00:02:B3:01:01:01'" >& 2
else
	. $CONFIG_HW_ETH0
    	$IFCONFIG $DEVICE hw ether $HWADDR
fi

CONFIG_ETH0=/etc/sys/ifcfg-eth0
DEFAULT_ROUTE_FLAG=
if [ ! -f "$CONFIG_ETH0" -o ! -r "$CONFIG_ETH0" ] ; then
	$ECHO " Some Error in FileSystem, so use DHCP for WAN Port" >& 2
	/sbin/udhcpc -h `hostname` -i eth0 -s /etc/udhcpc.script &
#	exit 1
else
	. $CONFIG_ETH0
	if [ $IPPROTO = "DHCP" ]; then
		echo " WAN Port use DHCP client to config" >& 2
		/sbin/udhcpc -h `hostname` -i eth0 -s /etc/udhcpc.script &
		/sbin/route add default device eth0
	else 
		if [ $IPPROTO = "PPPOE" ]; then
			echo " WAN Port use PPPOE to config" >& 2
			/usr/sbin/adsl-connect > /tmp/PPPoE.log &
			# Check status when ADSL start up.
			# $CGI_BIN/ppp_up_check /tmp/PPPoE.log &
		else
			echo " WAN Port use static configuration" >& 2
		    	$IFCONFIG $DEVICE $IPADDR broadcast $BROADCAST netmask $NETMASK
		    	echo "$IPADDR      $HOST_NAME.$DOMAIN_NAME" >> /etc/hosts

			DEFAULT_ROUTE_FLAG=`/sbin/route | grep default`
			if [ $DEFAULT_ROUTE_FLAG ] ; then
				/sbin/route delete default
			fi
			/sbin/route add default device eth0
	     	fi	
	fi 
fi
