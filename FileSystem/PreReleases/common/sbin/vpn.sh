# /bin/sh
# VPN Masquerade script
# Lizhijie, 2006.12.11
#

IPTABLE=/usr/sbin/iptables
CONFIG_VPN=/etc/iptables/vpn.conf
# eth0 or pppx
DEVICE=eth0
# Add or DEL from runtime

if [ $1 = "ADD" ]; then
	OP=-A
else
	OP=-D
fi

. $CONFIG_VPN

if [ $Pptp_Gateway = "" ]; then
	echo " No VPN is available now" >& 2
else 
	echo " VPN tunnel for Gateway $Pptp_Gateway and PPTP Host $Pptp_LocalIp is $1" >& 2

	echo " $IPTABLE -t nat $OP PREROUTING -i $DEVICE -d $Pptp_Gateway -p tcp --dport 1723  -j DNAT --to-destination $Pptp_LocalIp:1723" >& 2
	$IPTABLE -t nat $OP PREROUTING -i $DEVICE -d $Pptp_Gateway -p tcp --dport 1723  -j DNAT --to-destination $Pptp_LocalIp:1723

	echo " $IPTABLE -t nat $OP PREROUTING -i $DEVICE -d $Pptp_Gateway -p 47 -j DNAT --to-destination $Pptp_LocalIp" >& 2
	$IPTABLE -t nat $OP PREROUTING -i $DEVICE -d $Pptp_Gateway -p 47 -j DNAT --to-destination $Pptp_LocalIp


	echo " $IPTABLE -t nat $OP POSTROUTING -o $DEVICE -s $Pptp_LocalIp -p tcp --dport 1723  -j SNAT --to-source $Pptp_Gateway:1723" >& 2
	$IPTABLE -t nat $OP POSTROUTING -o $DEVICE -s $Pptp_LocalIp -p tcp --dport 1723  -j SNAT --to-source $Pptp_Gateway:1723

	echo " $IPTABLE -t nat $OP POSTROUTING -o $DEVICE -s $Pptp_LocalIp -p 47 -j SNAT --to-source $Pptp_Gateway" >& 2
	$IPTABLE -t nat $OP POSTROUTING -o $DEVICE -s $Pptp_LocalIp -p 47 -j SNAT --to-source $Pptp_Gateway
fi	
