#/bin/sh
# shell script for pppoe server
ifconfig eth1 0.0.0.0 up promisc

pppoe-server -I eth0 -L 192.168.5.1 -R 192.168.5.111 -N 32

