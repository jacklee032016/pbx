# /bin/sh
# LAN configuration scripts : both for MAC and IP address
# Lizhijie, 2006.08.08
#

IFCONFIG=/sbin/ifconfig

CONFIG_HW_ETH1=/etc/sys/hwcfg-eth1
if [ ! -f "$CONFIG_HW_ETH1" -o ! -r "$CONFIG_HW_ETH1" ] ; then
	$ECHO " WAN interface use default hardware interface '00:02:B3:02:02:02'" >& 2
else
	. $CONFIG_HW_ETH1
    	$IFCONFIG $DEVICE hw ether $HWADDR
fi

CONFIG_ETH1=/etc/sys/ifcfg-eth1
if [ ! -f "$CONFIG_ETH1" -o ! -r "$CONFIG_ETH1" ] ; then
	$ECHO " Some Error in FileSystem, so use DHCP for LAN Port" >& 2
	/sbin/udhcpc -h `hostname` -i eth1 -s /etc/udhcpc.script &
#	exit 1
else
	. $CONFIG_ETH1

	if [ $IPPROTO = "DHCP" ]; then
		echo " LAN Port use DHCP client to config" >& 2
		/sbin/udhcpc -h `hostname` -i $DEVICE -s /etc/udhcpc.script &
	else 
		echo " LAN Port use static configuration" >& 2
		echo " $IFCONFIG $DEVICE $IPADDR broadcast $BROADCAST netmask $NETMASK" >& 2
		echo "$IPADDR      $HOST_NAME.$DOMAIN_NAME" >> /etc/hosts
		$IFCONFIG $DEVICE $IPADDR broadcast $BROADCAST netmask $NETMASK
    fi	
fi

if [ $DHCPSERVICE = "YES" ]; then
	echo " startup DHCP service on LAN Port" >& 2
	/usr/sbin/udhcpd &
fi	
