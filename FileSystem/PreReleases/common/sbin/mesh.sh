# /bin/sh
# scripts start all MESH Drivers
# Li Zhijie, 2006.11.06
# $Id: mesh.sh,v 1.1.1.1 2006/11/29 18:55:33 lizhijie Exp $

IFCONFIG=/sbin/ifconfig
MESHCONFIG=/sbin/meshconfig
MESHPRIV=/sbin/meshpriv
START=/sbin/mgrconfig

CONFIG_WIFI=/etc/sys/mesh.conf
if [ ! -f "$CONFIG_WIFI" -o ! -r "$CONFIG_WIFI" ] ; then
	$ECHO " Some Error in FileSystem, No configuration Inforamtion for WIFI" >& 2
	exit 1
fi
. $CONFIG_WIFI
if [ ! $WIFI_ONBOOT = "YES" ]; then
	exit 1
fi 

echo "Load MESH Drivers....."
insmod hal
insmod mesh
insmod mac
insmod phy
echo "Drivers Loaded"

CONFIG_ETH_WIFI=/etc/sys/ifcfg-mesh
if [ ! -f "$CONFIG_ETH_WIFI" -o ! -r "$CONFIG_ETH_WIFI" ] ; then
	$ECHO " Some Error in FileSystem, so use DHCP for LAN Port" >& 2
	exit 1
fi

	. $CONFIG_ETH_WIFI

var0=0

while [ "$var0" -lt "$MESH_NUM" ]
do
#	echo "Start All Module of MESH....."
	DEVICE=`expr $var0 + 1`
	
#	echo ""
#	echo "No. $var0 MESH Device"
	WIFI_MODE=$(eval "echo WIFI_MODE_$var0")	
	eval WIFI_MODE=\$$WIFI_MODE
	$MESHCONFIG $DEVICE mode $WIFI_MODE
	
	WIFI_ESSID=$(eval "echo WIFI_ESSID_$var0")	
	eval WIFI_ESSID=\$$WIFI_ESSID
	$MESHCONFIG $DEVICE essid $WIFI_ESSID
	
	WIFI_NAME=$(eval "echo WIFI_NAME_$var0")	
	eval WIFI_NAME=\$$WIFI_NAME
	$MESHCONFIG $DEVICE nick $WIFI_NAME
	
	# phy mode must be set before key
	
	WIFI_PHY_MODE=$(eval "echo WIFI_PHY_MODE_$var0")	
	eval WIFI_PHY_MODE=\$$WIFI_PHY_MODE
	$MESHPRIV $DEVICE mode $WIFI_PHY_MODE
	
	WIFI_CHANNEL=$(eval "echo WIFI_CHANNEL_$var0")	
	eval WIFI_CHANNEL=\$$WIFI_CHANNEL
	$MESHCONFIG $DEVICE channel $WIFI_CHANNEL
	
	WIFI_RATE=$(eval "echo WIFI_RATE_$var0")	
	eval WIFI_RATE=\$$WIFI_RATE
	$MESHCONFIG $DEVICE rate $WIFI_RATE

	WIFI_KEY=$(eval "echo WIFI_KEY_$var0")	
	eval WIFI_KEY=\$$WIFI_KEY
	# $MESHCONFIG $DEVICE key $WIFI_KEY
	
	echo "MESH Device $DEVICE : $WIFI_MODE,ESSID($WIFI_ESSID),NICK_NAME($WIFI_NAME),PHY($WIFI_PHY_MODE),Channel($WIFI_CHANNEL),Rate($WIFI_RATE)"
	var0=`expr $var0 + 1`
done


if [ $MESH_PORTAL = "YES" ]; then
	echo ""
	echo "Enable MESH Portal Device on Ethernet $MESH_PORTAL_DEVICE"
	$IFCONFIG $MESH_PORTAL_DEVICE down
	$START 0 portal enable $MESH_PORTAL_DEVICE 

	$IFCONFIG mesh up

	$IFCONFIG $MESH_PORTAL_DEVICE 0.0.0.0
	$IFCONFIG $MESH_PORTAL_DEVICE up

	$IFCONFIG mbr $IPADDR netmask $NETMASK broadcast $BROADCAST
fi


$START 0 start Yes
echo "MESH Module Started"
