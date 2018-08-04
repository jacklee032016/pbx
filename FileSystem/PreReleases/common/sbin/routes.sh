# /bin/sh
# add routes when startup
# Lizhijie, 2006.08.31
#

ROUTE=/sbin/route
CONFIG_ROUTES=/etc/sys/routes.conf

. $CONFIG_ROUTES

var0=0

while [ "$var0" -lt "$ROUTE_NUM" ]
do
	echo "No. $var0 Route Entry"
	ROUTE_TYPE=$(eval "echo ROUTE_TYPE_$var0")	
	eval ROUTE_TYPE=\$$ROUTE_TYPE
	DESTIN=$(eval "echo DESTIN_${var0}")
	eval DESTIN=\$$DESTIN
	GATEWAY=$(eval "echo GATEWAY_${var0}")
	eval GATEWAY=\$$GATEWAY
	NETMASK=$(eval "echo NETMASK_${var0}")
	eval NETMASK=\$$NETMASK
	INTERFACE=$(eval "echo INTERFACE_${var0}")
	eval INTERFACE=\$$INTERFACE
	echo "$ROUTE_TYPE $DESTIN $GATEWAY $NETMASK $INTERFACE"
	echo ""
	if [ $ROUTE_TYPE = "default" ]; then
		echo "First time for HD Disk $DEV Mount"
		$ROUTE add $ROUTE_TYPE gw $GATEWAY
	else
		if [ $ROUTE_TYPE = "host" ];then
			$ROUTE add -host $DESTIN gw $GATEWAY
		else
			$ROUTE add -net $DESTIN netmask $NETMASK gw $GATEWAY
		fi
	fi
	
	var0=`expr $var0 + 1`
done

