#!/bin/bash
#
# RCSID $Id: kernel.patch.gen.sh,v 1.1.1.1 2006/11/30 16:28:46 lizhijie Exp $

patchdir=`pwd`
kernelsrc=/usr/src/linux
[ "$1~" = "~" ] || kernelsrc=$1
cd $kernelsrc
# clean out destination file for all patch
#echo "">$patchdir/all

# find files to patch and loop
for i in  `find . -name '*.preipsec'`
do

# strip off '.preipsec' suffix
j=${i%.preipsec}

# strip off './' prefix
k=${j#\.\/}

# single unified diff
#diff -u $i $j >>$patchdir/all

# convert '/' in filename to '.' to avoid subdirectories
sed -e 's/\//\./g' << EOI > /tmp/t
$k
EOI
l=`cat /tmp/t`
rm -f /tmp/t

# *with* path from source root
#echo do diff -u $i $j '>' $patchdir/$l
echo found $i
echo "RCSID \$Id: kernel.patch.gen.sh,v 1.1.1.1 2006/11/30 16:28:46 lizhijie Exp $" >$patchdir/$l
diff -u $i $j >>$patchdir/$l

done

#
# $Log: kernel.patch.gen.sh,v $
# Revision 1.1.1.1  2006/11/30 16:28:46  lizhijie
# AS800 Kernel
#
# Revision 1.1.1.1  2006/01/10 06:01:25  lizhijie
# kernel and driver for DUX project
#
# Revision 1.1.1.1  2005/07/08 09:46:24  lizhijie
# Linux-2.4.24 integrated Intel Access Library 2.0
#
# Revision 1.1.1.1  2004/08/17 02:50:02  lizhijie
# linux-2.4.24 for IXP422
#
# Revision 1.4  1999/04/06 04:54:30  rgb
# Fix/Add RCSID Id: and Log: bits to make PHMDs happy.  This includes
# patch shell fixes.
#
#
