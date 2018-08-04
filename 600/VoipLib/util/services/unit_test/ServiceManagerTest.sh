#!/bin/bash
#
#   $Id: ServiceManagerTest.sh,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $
#

MACHINE="$(uname).$(uname -m)"
APP=ServiceManagerTest
if [ -x ./bin.debug.${MACHINE} ]
then

for FLAGS in "-f"
do

echo -e "./bin.debug.${MACHINE}/${APP} $FLAGS"
echo "-----------------------------------------------------"

./bin.debug.${MACHINE}/${APP} ${FLAGS}

done

fi
