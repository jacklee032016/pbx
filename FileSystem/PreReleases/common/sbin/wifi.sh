# /bin/sh
# WIFI configuration scripts : Add drivers and config all
# Lizhijie, 2006.09.16
#

IFCONFIG=/sbin/ifconfig
WIFICONFIG=/sbin/wificonfig
WIFIPRIV=/sbin/wifipriv

CONFIG_WIFI=/etc/sys/wifi.conf
if [ ! -f "$CONFIG_WIFI" -o ! -r "$CONFIG_WIFI" ] ; then
	$ECHO " Some Error in FileSystem, No configuration Inforamtion for WIFI" >& 2
	exit 1
fi

. $CONFIG_WIFI
if [ ! $WIFI_ONBOOT = "YES" ]; then
	/sbin/mesh.sh
	exit 1
fi 

echo "Loading WIFI Drivers....."
insmod wifihw
insmod wifimac
insmod wifionoe
insmod wifiphy

if [ $? -ne 0 ] ; then
	echo "No Wifi Hardware found, so remove loaded Wifi modules"
	rmmod wifionoe
	rmmod wifimac
	rmmod wifihw
	exit 1
fi


insmod wifixauth
insmod wifiacl
# dot1x must be load before radius
insmod wifidot1x
insmod wifiradius
insmod wificryptwep

insmod wificrypttkip
insmod wificryptccmp

CONFIG_ETH_WIFI=/etc/sys/ifcfg-wifi
if [ ! -f "$CONFIG_ETH_WIFI" -o ! -r "$CONFIG_ETH_WIFI" ] ; then
	$ECHO " Some Error in FileSystem, so use DHCP for LAN Port" >& 2
	exit 1
fi

	. $CONFIG_ETH_WIFI

$WIFICONFIG $DEVICE mode $WIFI_MODE
$WIFICONFIG $DEVICE essid $WIFI_ESSID
$WIFICONFIG $DEVICE nickname $WIFI_NAME

# phy mode must be set before 
$WIFIPRIV $DEVICE mode $WIFI_PHY_MODE
$WIFICONFIG $DEVICE channel $WIFI_CHANNEL

$WIFICONFIG $DEVICE key $WIFI_KEY

if [ $WIFI_AODV = "YES" ]; then
		echo " startup Wireless AODV(Ad-Hoc On-Demand Vector) Router on WIFI Port" >& 2
		$WIFICONFIG $DEVICE mode Ad-Hoc
		$IFCONFIG $DEVICE $IPADDR netmask $NETMASK broadcast $BROADCAST
		echo "$IPADDR      $HOST_NAME.$DOMAIN_NAME" >> /etc/hosts
		
		echo "1" > /proc/sys/net/ipv4/ip_forward
		echo "0" > /proc/sys/net/ipv4/route/min_delay

		insmod route_core paramLocalSubnet=$IPADDR/$NETMASK
		insmod ip_route paramDevName=$DEVICE                            
else 

	if [ $IPPROTO = "DHCP" ]; then
		echo " WIFI Port use DHCP client to config" >& 2
		/sbin/udhcpc -h `hostname` -i $DEVICE -s /etc/udhcpc.script &
	else 
		echo " WIFI Port use static configuration" >& 2
	    	$IFCONFIG $DEVICE $IPADDR broadcast $BROADCAST netmask $NETMASK
		echo "$IPADDR      $HOST_NAME.$DOMAIN_NAME" >> /etc/hosts

     	fi	
    
	if [ $DHCPSERVICE = "YES" ]; then
		echo " startup DHCP service on WIFI Port" >& 2
		/usr/sbin/wdhcpd &
	fi	

fi	

echo "Rate is set as $WIFI_RATE"
$WIFICONFIG $DEVICE rate $WIFI_RATE

######################### following is for authen and crypto  #####################
WIFI_CONFIG_AUTHEN=/etc/wifi/authen.conf

var0=0

if [ ! -f "$WIFI_CONFIG_AUTHEN" -o ! -r "$WIFI_CONFIG_AUTHEN" ] ; then
	$ECHO " Some Error in FileSystem, No WIFI Authentication Configuration file is found" >& 2
else

	. $WIFI_CONFIG_AUTHEN
	
	if [ $WIFI_AUTHEN_MODE = "1" ]; then
		echo "Wifi Authentication is disabled"
	else
	
		echo "Wifi Key Mode : $WIFI_AUTHEN_KEY_POLICY"
		
		while [ "$var0" -lt "$WIFI_AUTHEN_KEY_NUM" ]
		do
			WIFI_AUTHEN_KEY=$(eval "echo WIFI_AUTHEN_KEY_$var0")	
			eval WIFI_AUTHEN_KEY=\$$WIFI_AUTHEN_KEY
		
			var0=`expr $var0 + 1`
			echo "     No $var0 WEP Key is set as $WIFI_AUTHEN_KEY"
			$WIFICONFIG $DEVICE key s:$WIFI_AUTHEN_KEY [$var0] $WIFI_AUTHEN_KEY_POLICY
		done
		
		echo "Wifi Authentication mode is $WIFI_AUTHEN_MODE"
		$WIFIPRIV $DEVICE authmode $WIFI_AUTHEN_MODE
		$WIFIPRIV $DEVICE get_authmode 
	fi
fi


######################### following is for Wifi MAC Filters  #####################
WIFI_CONFIG_ACL=/etc/wifi/acl.conf

var0=0

if [ ! -f "$WIFI_CONFIG_ACL" -o ! -r "$WIFI_CONFIG_ACL" ] ; then
	$ECHO " Some Error in FileSystem, No WIFI ACL Configuration file is found" >& 2
else

	. $WIFI_CONFIG_ACL
	
	if [ $WIFI_ACL_POLICY = "NO" ]; then
		echo "Wifi MAC Filter is disabled"
	else
		# default is ALLOW
		POLICY=1
		if [ $WIFI_ACL_POLICY = "DENY" ]; then
			POLICY=2
		fi
		$WIFIPRIV $DEVICE setparam 17 $POLICY 
			
		echo "Wifi MAC Filter Policy : $WIFI_ACL_POLICY"
		
		while [ "$var0" -lt "$WIFI_ACL_MAC_NUM" ]
		do
			WIFI_ACL_MAC=$(eval "echo WIFI_ACL_MAC_$var0")	
			eval WIFI_ACL_MAC=\$$WIFI_ACL_MAC
		
			$WIFIPRIV $DEVICE addmac $WIFI_ACL_MAC 
			var0=`expr $var0 + 1`
			echo "     No $var0 MAC Address $WIFI_ACL_MAC"
		done
		
		echo "Wifi Ploicy is $WIFI_ACL_POLICY"
		echo "Following MAC Addresses is activating"
		cat /proc/sys/net/$WIFI_DEVICE/ACL/macs

	fi
fi	
