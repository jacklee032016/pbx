# /bin/sh
# Æô¶¯UPNPDµÄ½Å±¾

CONFIG_ETH0=/etc/sys/ifcfg-eth0
. $CONFIG_ETH0

WAN_PORT=eth0
LAN_PORT=eth1
if [ $IPPROTO = "PPPOE" ]; then
	WAN_PORT=ppp0
fi	

/sbin/route add -net 239.0.0.0 netmask 255.0.0.0 $WAN_PORT 
/usr/sbin/upnpd  $WAN_PORT  $LAN_PORT
