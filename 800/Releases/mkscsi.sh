#!/bin/sh

cc=0
for i in a b c d e
do
echo "cc = $cc"
echo "mknod sd$i b 8 $cc"
mknod sd"$i" b 8 "$cc"
for j in 1 2 3 4 5 6 7 8
do
	minor=`expr "$cc" + "$j"`
	echo "mknod sd$i$j b 8 $minor"
	mknod sd"$i""$j" b 8 "$minor"
done
cc=`expr $cc + 16`
done
echo enjoy it

