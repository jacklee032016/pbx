#!/bin/bash

mkdir -p tmp
cd tmp

tar xvzf ../assist_pppd_arm*.tar.gz
tar xvzf ../assist_pptpd_arm*.tar.gz

cp -R ../releases/*  .
find . -name 'CVS' -print -exec /bin/rm -rf {} \;

#BUILDTIME=`TZ=UTC date -u "+%Y_%m_%d-%H_%M"`
BUILDTIME=`TZ=UTC date -u "+%Y_%m_%d"`

tar cvzf ../patchOf0407FileSystem_$BUILDTIME.tar.gz *
cd ../
rm -rf tmp
echo "build time : $BUILDTIME"
