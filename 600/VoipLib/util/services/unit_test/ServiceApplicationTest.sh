#!/bin/bash
#
#   $Id: ServiceApplicationTest.sh,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $
#

MACHINE="$(uname).$(uname -m)"

if [ -x ./bin.debug.${MACHINE} ]
then

echo -e "./bin.debug.${MACHINE}/ServiceApplicationTest -n -f -x"
./bin.debug.${MACHINE}/ServiceApplicationTest -n -f -x

echo -e "./bin.debug.${MACHINE}/ServiceApplicationTest -n --config=ServiceApplicationTest.rc"
./bin.debug.${MACHINE}/ServiceApplicationTest -n --config=ServiceApplicationTest.rc

echo -e "./bin.debug.${MACHINE}/ServiceApplicationTest -n --config=../../../translators/h323/siph323csgw/siph323csgw.conf"
./bin.debug.${MACHINE}/ServiceApplicationTest -n --config=../../../translators/h323/siph323csgw/siph323csgw.conf

echo -e "./bin.debug.${MACHINE}/ServiceApplicationTest --config=../../../translators/h323/siph323csgw/siph323csgw.conf"
./bin.debug.${MACHINE}/ServiceApplicationTest -n --config=../../../translators/h323/siph323csgw/siph323csgw.conf

else

exit 1

fi
